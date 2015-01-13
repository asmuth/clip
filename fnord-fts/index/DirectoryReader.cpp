/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "DirectoryReader.h"
#include "_DirectoryReader.h"
#include "Directory.h"
#include "ReadOnlyDirectoryReader.h"
#include "IndexWriter.h"
#include "_IndexWriter.h"
#include "IndexCommit.h"
#include "IndexDeletionPolicy.h"
#include "IndexFileDeleter.h"
#include "IndexFileNames.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "SegmentInfos.h"
#include "SegmentInfo.h"
#include "SegmentReader.h"
#include "DefaultSimilarity.h"
#include "ReadOnlySegmentReader.h"
#include "SegmentMergeInfo.h"
#include "Lock.h"
#include "FieldCache.h"
#include "MiscUtils.h"

namespace Lucene {

DirectoryReader::DirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& sis, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor) {
    normsCache = MapStringByteArray::newInstance();
    _maxDoc = 0;
    _numDocs = -1;
    _hasDeletions = false;
    synced = HashSet<String>::newInstance();
    stale = false;
    rollbackHasChanges = false;

    this->_directory = directory;
    this->readOnly = readOnly;
    this->segmentInfos = sis;
    this->deletionPolicy = deletionPolicy;
    this->termInfosIndexDivisor = termInfosIndexDivisor;

    if (!readOnly) {
        // We assume that this segments_N was previously properly sync'd
        HashSet<String> files(sis->files(directory, true));
        synced.addAll(files.begin(), files.end());
    }

    // To reduce the chance of hitting FileNotFound (and having to retry), we open segments in
    // reverse because IndexWriter merges & deletes the newest segments first.
    Collection<SegmentReaderPtr> readers(Collection<SegmentReaderPtr>::newInstance(sis->size()));

    for (int32_t i = sis->size() - 1; i >= 0; --i) {
        bool success = false;
        LuceneException finally;
        try {
            readers[i] = SegmentReader::get(readOnly, sis->info(i), termInfosIndexDivisor);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            // Close all readers we had opened
            for (Collection<SegmentReaderPtr>::iterator closeReader = readers.begin(); closeReader != readers.end(); ++closeReader) {
                try {
                    if (*closeReader) {
                        (*closeReader)->close();
                    }
                } catch (...) {
                    // keep going - we want to clean up as much as possible
                }
            }
        }
        finally.throwException();
    }

    _initialize(readers);
}

DirectoryReader::DirectoryReader(const IndexWriterPtr& writer, const SegmentInfosPtr& infos, int32_t termInfosIndexDivisor) {
    normsCache = MapStringByteArray::newInstance();
    _maxDoc = 0;
    _numDocs = -1;
    _hasDeletions = false;
    synced = HashSet<String>::newInstance();
    stale = false;
    rollbackHasChanges = false;

    this->_directory = writer->getDirectory();
    this->readOnly = true;
    this->segmentInfos = infos;
    this->segmentInfosStart = boost::dynamic_pointer_cast<SegmentInfos>(infos->clone());
    this->termInfosIndexDivisor = termInfosIndexDivisor;

    if (!readOnly) {
        // We assume that this segments_N was previously properly sync'd
        HashSet<String> files(infos->files(_directory, true));
        synced.addAll(files.begin(), files.end());
    }

    // IndexWriter synchronizes externally before calling us, which ensures infos will not change; so there's
    // no need to process segments in reverse order
    int32_t numSegments = infos->size();
    Collection<SegmentReaderPtr> readers(Collection<SegmentReaderPtr>::newInstance(numSegments));
    DirectoryPtr dir(writer->getDirectory());
    int32_t upto = 0;

    for (int32_t i = 0; i < numSegments; ++i) {
        bool success = false;
        LuceneException finally;
        try {
            SegmentInfoPtr info(infos->info(i));
            if (info->dir == dir) {
                readers[upto++] = boost::dynamic_pointer_cast<SegmentReader>(writer->readerPool->getReadOnlyClone(info, true, termInfosIndexDivisor));
            }
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            // Close all readers we had opened
            for (--upto; upto >= 0; --upto) {
                try {
                    if (readers[upto]) {
                        readers[upto]->close();
                    }
                } catch (...) {
                    // keep going - we want to clean up as much as possible
                }
            }
        }
        finally.throwException();
    }

    this->_writer = writer;

    if (upto < readers.size()) {
        // This means some segments were in a foreign Directory
        readers.resize(upto);
    }

    _initialize(readers);
}

DirectoryReader::DirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& infos, Collection<SegmentReaderPtr> oldReaders,
                                 Collection<int32_t> oldStarts, MapStringByteArray oldNormsCache, bool readOnly,
                                 bool doClone, int32_t termInfosIndexDivisor) {
    normsCache = MapStringByteArray::newInstance();
    _maxDoc = 0;
    _numDocs = -1;
    _hasDeletions = false;
    synced = HashSet<String>::newInstance();
    stale = false;
    rollbackHasChanges = false;

    this->_directory = directory;
    this->readOnly = readOnly;
    this->segmentInfos = infos;
    this->termInfosIndexDivisor = termInfosIndexDivisor;
    if (!readOnly) {
        // We assume that this segments_N was previously properly sync'd
        HashSet<String> files(infos->files(directory, true));
        synced.addAll(files.begin(), files.end());
    }

    // we put the old SegmentReaders in a map, that allows us to lookup a reader using its segment name
    MapStringInt segmentReaders(MapStringInt::newInstance());

    if (oldReaders) {
        int32_t segReader = 0;
        // create a Map SegmentName->SegmentReader
        for (Collection<SegmentReaderPtr>::iterator reader = oldReaders.begin(); reader != oldReaders.end(); ++reader) {
            segmentReaders.put((*reader)->getSegmentName(), segReader++);
        }
    }

    Collection<SegmentReaderPtr> newReaders(Collection<SegmentReaderPtr>::newInstance(infos->size()));

    // remember which readers are shared between the old and the re-opened DirectoryReader - we have to incRef those readers
    Collection<uint8_t> readerShared(Collection<uint8_t>::newInstance(infos->size()));

    for (int32_t i = infos->size() - 1; i >= 0; --i) {
        // find SegmentReader for this segment
        MapStringInt::iterator oldReaderIndex = segmentReaders.find(infos->info(i)->name);
        if (oldReaderIndex == segmentReaders.end()) {
            // this is a new segment, no old SegmentReader can be reused
            newReaders[i].reset();
        } else {
            // there is an old reader for this segment - we'll try to reopen it
            newReaders[i] = oldReaders[oldReaderIndex->second];
        }

        bool success = false;
        LuceneException finally;
        try {
            SegmentReaderPtr newReader;
            if (!newReaders[i] || infos->info(i)->getUseCompoundFile() != newReaders[i]->getSegmentInfo()->getUseCompoundFile()) {
                // We should never see a totally new segment during cloning
                BOOST_ASSERT(!doClone);

                // this is a new reader; in case we hit an exception we can close it safely
                newReader = SegmentReader::get(readOnly, infos->info(i), termInfosIndexDivisor);
            } else {
                newReader = newReaders[i]->reopenSegment(infos->info(i), doClone, readOnly);
            }

            if (newReader == newReaders[i]) {
                // this reader will be shared between the old and the new one, so we must incRef it
                readerShared[i] = true;
                newReader->incRef();
            } else {
                readerShared[i] = false;
                newReaders[i] = newReader;
            }
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            for (++i; i < infos->size(); ++i) {
                if (newReaders[i]) {
                    try {
                        if (!readerShared[i]) {
                            // this is a new subReader that is not used by the old one, we can close it
                            newReaders[i]->close();
                        } else {
                            // this subReader is also used by the old reader, so instead closing we must decRef it
                            newReaders[i]->decRef();
                        }
                    } catch (...) {
                        // keep going - we want to clean up as much as possible
                    }
                }
            }
        }
        finally.throwException();
    }

    // initialize the readers to calculate maxDoc before we try to reuse the old normsCache
    _initialize(newReaders);

    // try to copy unchanged norms from the old normsCache to the new one
    if (oldNormsCache) {
        for (MapStringByteArray::iterator entry = oldNormsCache.begin(); entry != oldNormsCache.end(); ++entry) {
            if (!hasNorms(entry->first)) {
                continue;
            }

            ByteArray bytes(ByteArray::newInstance(maxDoc()));

            for (int32_t i = 0; i < subReaders.size(); ++i) {
                MapStringInt::iterator oldReaderIndex = segmentReaders.find(subReaders[i]->getSegmentName());

                // this SegmentReader was not re-opened, we can copy all of its norms
                if (oldReaderIndex != segmentReaders.end() && (oldReaders[oldReaderIndex->second] == subReaders[i]
                        || oldReaders[oldReaderIndex->second]->_norms.get(entry->first) == subReaders[i]->_norms.get(entry->first))) {
                    // we don't have to synchronize here: either this constructor is called from a SegmentReader, in which
                    // case no old norms cache is present, or it is called from MultiReader.reopen(), which is synchronized
                    MiscUtils::arrayCopy(entry->second.get(), oldStarts[oldReaderIndex->second], bytes.get(), starts[i], starts[i + 1] - starts[i]);
                } else {
                    subReaders[i]->norms(entry->first, bytes, starts[i]);
                }
            }

            normsCache.put(entry->first, bytes); // update cache
        }
    }
}

DirectoryReader::~DirectoryReader() {
}

void DirectoryReader::_initialize(Collection<SegmentReaderPtr> subReaders) {
    this->subReaders = subReaders;
    starts = Collection<int32_t>::newInstance(subReaders.size() + 1);
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        starts[i] = _maxDoc;
        _maxDoc += subReaders[i]->maxDoc(); // compute maxDocs

        if (subReaders[i]->hasDeletions()) {
            _hasDeletions = true;
        }
    }
    starts[subReaders.size()] = _maxDoc;

    if (!readOnly) {
        maxIndexVersion = SegmentInfos::readCurrentVersion(_directory);
    }
}

IndexReaderPtr DirectoryReader::open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, const IndexCommitPtr& commit, bool readOnly, int32_t termInfosIndexDivisor) {
    return newLucene<FindSegmentsOpen>(readOnly, deletionPolicy, termInfosIndexDivisor, newLucene<SegmentInfos>(), directory)->run(commit);
}

LuceneObjectPtr DirectoryReader::clone(const LuceneObjectPtr& other) {
    try {
        return DirectoryReader::clone(readOnly, other); // Preserve current readOnly
    } catch (LuceneException& e) {
        boost::throw_exception(RuntimeException(e.getError()));
    }
    return DirectoryReaderPtr();
}

LuceneObjectPtr DirectoryReader::clone(bool openReadOnly, const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    DirectoryReaderPtr newReader(doReopen(boost::dynamic_pointer_cast<SegmentInfos>(segmentInfos->clone()), true, openReadOnly));

    if (shared_from_this() != newReader) {
        newReader->deletionPolicy = deletionPolicy;
    }

    newReader->_writer = _writer;

    // If we're cloning a non-readOnly reader, move the writeLock (if there is one) to the new reader
    if (!openReadOnly && writeLock) {
        // In near real-time search, reader is always readonly
        BOOST_ASSERT(_writer.expired());
        newReader->writeLock = writeLock;
        newReader->_hasChanges = _hasChanges;
        newReader->_hasDeletions = _hasDeletions;
        writeLock.reset();
        _hasChanges = false;
    }

    return newReader;
}

IndexReaderPtr DirectoryReader::reopen() {
    // Preserve current readOnly
    return doReopen(readOnly, IndexCommitPtr());
}

IndexReaderPtr DirectoryReader::reopen(bool openReadOnly) {
    return doReopen(openReadOnly, IndexCommitPtr());
}

IndexReaderPtr DirectoryReader::reopen(const IndexCommitPtr& commit) {
    return doReopen(true, commit);
}

IndexReaderPtr DirectoryReader::doReopenFromWriter(bool openReadOnly, const IndexCommitPtr& commit) {
    BOOST_ASSERT(readOnly);

    if (!openReadOnly) {
        boost::throw_exception(IllegalArgumentException(L"a reader obtained from IndexWriter.getReader() can only be reopened with openReadOnly=true (got false)"));
    }

    if (commit) {
        boost::throw_exception(IllegalArgumentException(L"a reader obtained from IndexWriter.getReader() cannot currently accept a commit"));
    }

    return IndexWriterPtr(_writer)->getReader();
}

IndexReaderPtr DirectoryReader::doReopen(bool openReadOnly, const IndexCommitPtr& commit) {
    ensureOpen();

    BOOST_ASSERT(!commit || openReadOnly);

    IndexWriterPtr writer(_writer.lock());

    // If we were obtained by writer.getReader(), re-ask the writer to get a new reader.
    if (writer) {
        return doReopenFromWriter(openReadOnly, commit);
    } else {
        return doReopenNoWriter(openReadOnly, commit);
    }
}

IndexReaderPtr DirectoryReader::doReopenNoWriter(bool openReadOnly, const IndexCommitPtr& commit) {
    SyncLock syncLock(this);
    if (!commit) {
        if (_hasChanges) {
            // We have changes, which means we are not readOnly
            BOOST_ASSERT(!readOnly);
            // and we hold the write lock
            BOOST_ASSERT(writeLock);
            // so no other writer holds the write lock, which means no changes could have been done to the index
            BOOST_ASSERT(isCurrent());

            if (openReadOnly) {
                return boost::dynamic_pointer_cast<IndexReader>(clone(openReadOnly));
            } else {
                return shared_from_this();
            }
        } else if (isCurrent()) {
            if (openReadOnly != readOnly) {
                // Just fallback to clone
                return boost::dynamic_pointer_cast<IndexReader>(clone(openReadOnly));
            } else {
                return shared_from_this();
            }
        }
    } else {
        if (_directory != commit->getDirectory()) {
            boost::throw_exception(IOException(L"the specified commit does not match the specified Directory"));
        }
        if (segmentInfos && commit->getSegmentsFileName() == segmentInfos->getCurrentSegmentFileName()) {
            if (readOnly != openReadOnly) {
                // Just fallback to clone
                return boost::dynamic_pointer_cast<IndexReader>(clone(openReadOnly));
            } else {
                return shared_from_this();
            }
        }
    }

    return newLucene<FindSegmentsReopen>(shared_from_this(), openReadOnly, newLucene<SegmentInfos>(), _directory)->run(commit);
}

DirectoryReaderPtr DirectoryReader::doReopen(const SegmentInfosPtr& infos, bool doClone, bool openReadOnly) {
    SyncLock syncLock(this);
    if (openReadOnly) {
        return newLucene<ReadOnlyDirectoryReader>(_directory, infos, subReaders, starts, normsCache, doClone, termInfosIndexDivisor);
    } else {
        return newLucene<DirectoryReader>(_directory, infos, subReaders, starts, normsCache, false, doClone, termInfosIndexDivisor);
    }
}

int64_t DirectoryReader::getVersion() {
    ensureOpen();
    return segmentInfos->getVersion();
}

Collection<TermFreqVectorPtr> DirectoryReader::getTermFreqVectors(int32_t docNumber) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    return subReaders[i]->getTermFreqVectors(docNumber - starts[i]); // dispatch to segment
}

TermFreqVectorPtr DirectoryReader::getTermFreqVector(int32_t docNumber, const String& field) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    return subReaders[i]->getTermFreqVector(docNumber - starts[i], field);
}

void DirectoryReader::getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    subReaders[i]->getTermFreqVector(docNumber - starts[i], field, mapper);
}

void DirectoryReader::getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    subReaders[i]->getTermFreqVector(docNumber - starts[i], mapper);
}

bool DirectoryReader::isOptimized() {
    ensureOpen();
    return (segmentInfos->size() == 1 && !hasDeletions());
}

int32_t DirectoryReader::numDocs() {
    // Don't call ensureOpen() here (it could affect performance)

    // NOTE: multiple threads may wind up init'ing numDocs... but that's harmless
    if (_numDocs == -1) { // check cache
        int32_t n = 0; // cache miss - recompute
        for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
            n += (*reader)->numDocs();    // sum from readers
        }
        _numDocs = n;
    }
    return _numDocs;
}

int32_t DirectoryReader::maxDoc() {
    // Don't call ensureOpen() here (it could affect performance)
    return _maxDoc;
}

DocumentPtr DirectoryReader::document(int32_t n, const FieldSelectorPtr& fieldSelector) {
    ensureOpen();
    int32_t i = readerIndex(n); // find segment num
    return subReaders[i]->document(n - starts[i], fieldSelector); // dispatch to segment reader
}

bool DirectoryReader::isDeleted(int32_t n) {
    // Don't call ensureOpen() here (it could affect performance)
    int32_t i = readerIndex(n); // find segment num
    return subReaders[i]->isDeleted(n - starts[i]); // dispatch to segment reader
}

bool DirectoryReader::hasDeletions() {
    // Don't call ensureOpen() here (it could affect performance)
    return _hasDeletions;
}

void DirectoryReader::doDelete(int32_t docNum) {
    _numDocs = -1; // invalidate cache
    int32_t i = readerIndex(docNum); // find segment num
    subReaders[i]->deleteDocument(docNum - starts[i]); // dispatch to segment reader
    _hasDeletions = true;
}

void DirectoryReader::doUndeleteAll() {
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        (*reader)->undeleteAll();
    }
    _hasDeletions = false;
    _numDocs = -1; // invalidate cache
}

int32_t DirectoryReader::readerIndex(int32_t n) {
    return readerIndex(n, this->starts, this->subReaders.size());
}

int32_t DirectoryReader::readerIndex(int32_t n, Collection<int32_t> starts, int32_t numSubReaders) {
    // Binary search to locate reader
    Collection<int32_t>::iterator reader = std::upper_bound(starts.begin(), starts.begin() + numSubReaders, n);
    return (int32_t)(std::distance(starts.begin(), reader) - 1);
}

bool DirectoryReader::hasNorms(const String& field) {
    ensureOpen();
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        if ((*reader)->hasNorms(field)) {
            return true;
        }
    }
    return false;
}

ByteArray DirectoryReader::norms(const String& field) {
    SyncLock syncLock(this);
    ensureOpen();
    ByteArray bytes(normsCache.get(field));
    if (bytes) {
        return bytes;    // cache hit
    }
    if (!hasNorms(field)) {
        return ByteArray();
    }

    bytes = ByteArray::newInstance(maxDoc());
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        subReaders[i]->norms(field, bytes, starts[i]);
    }
    normsCache.put(field, bytes); // update cache
    return bytes;
}

void DirectoryReader::norms(const String& field, ByteArray norms, int32_t offset) {
    SyncLock syncLock(this);
    ensureOpen();
    ByteArray bytes(normsCache.get(field));
    if (!bytes && !hasNorms(field)) {
        MiscUtils::arrayFill(norms.get(), offset, norms.size(), DefaultSimilarity::encodeNorm(1.0));
    } else if (bytes) { // cache hit
        MiscUtils::arrayCopy(bytes.get(), 0, norms.get(), offset, maxDoc());
    } else {
        for (int32_t i = 0; i < subReaders.size(); ++i) { // read from segments
            subReaders[i]->norms(field, norms, offset + starts[i]);
        }
    }
}

void DirectoryReader::doSetNorm(int32_t doc, const String& field, uint8_t value) {
    {
        SyncLock normsLock(&normsCache);
        normsCache.remove(field); // clear cache
    }
    int32_t i = readerIndex(doc); // find segment num
    subReaders[i]->setNorm(doc - starts[i], field, value); // dispatch
}

TermEnumPtr DirectoryReader::terms() {
    ensureOpen();
    return newLucene<MultiTermEnum>(shared_from_this(), Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end()), starts, TermPtr());
}

TermEnumPtr DirectoryReader::terms(const TermPtr& t) {
    ensureOpen();
    return newLucene<MultiTermEnum>(shared_from_this(), Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end()), starts, t);
}

int32_t DirectoryReader::docFreq(const TermPtr& t) {
    ensureOpen();
    int32_t total = 0; // sum freqs in segments
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        total += (*reader)->docFreq(t);
    }
    return total;
}

TermDocsPtr DirectoryReader::termDocs() {
    ensureOpen();
    return newLucene<MultiTermDocs>(shared_from_this(), Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end()), starts);
}

TermPositionsPtr DirectoryReader::termPositions() {
    ensureOpen();
    return newLucene<MultiTermPositions>(shared_from_this(), Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end()), starts);
}

void DirectoryReader::acquireWriteLock() {
    if (readOnly) {
        // NOTE: we should not reach this code with the core IndexReader classes;
        // however, an external subclass of IndexReader could reach this.
        ReadOnlySegmentReader::noWrite();
    }

    if (segmentInfos) {
        ensureOpen();
        if (stale) {
            boost::throw_exception(StaleReaderException(L"IndexReader out of date and no longer valid for delete, undelete, or setNorm operations"));
        }

        if (!writeLock) {
            LockPtr writeLock(_directory->makeLock(IndexWriter::WRITE_LOCK_NAME));
            if (!writeLock->obtain((int32_t)IndexWriter::WRITE_LOCK_TIMEOUT)) { // obtain write lock
                boost::throw_exception(LockObtainFailedException(L"Index locked for write: " + writeLock->toString()));
            }
            this->writeLock = writeLock;

            // we have to check whether index has changed since this reader was opened.
            // if so, this reader is no longer valid for deletion
            if (SegmentInfos::readCurrentVersion(_directory) > maxIndexVersion) {
                stale = true;
                this->writeLock->release();
                this->writeLock.reset();
                boost::throw_exception(StaleReaderException(L"IndexReader out of date and no longer valid for delete, undelete, or setNorm operations"));
            }
        }
    }
}

void DirectoryReader::doCommit(MapStringString commitUserData) {
    if (_hasChanges) {
        segmentInfos->setUserData(commitUserData);

        // Default deleter (for backwards compatibility) is KeepOnlyLastCommitDeleter
        IndexFileDeleterPtr deleter(newLucene<IndexFileDeleter>(_directory, deletionPolicy ? deletionPolicy : newLucene<KeepOnlyLastCommitDeletionPolicy>(), segmentInfos, InfoStreamPtr(), DocumentsWriterPtr(), synced));
        segmentInfos->updateGeneration(deleter->getLastSegmentInfos());

        // Checkpoint the state we are about to change, in case we have to roll back
        startCommit();

        bool success = false;
        LuceneException finally;
        try {
            for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
                (*reader)->commit();
            }

            // Sync all files we just wrote
            HashSet<String> files(segmentInfos->files(_directory, false));
            for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
                if (!synced.contains(*fileName)) {
                    BOOST_ASSERT(_directory->fileExists(*fileName));
                    _directory->sync(*fileName);
                    synced.add(*fileName);
                }
            }

            segmentInfos->commit(_directory);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }

        if (!success) {
            // Rollback changes that were made to SegmentInfos but failed to get [fully]
            // committed.  This way this reader instance remains consistent (matched to what's
            // actually in the index)
            rollbackCommit();

            // Recompute deletable files & remove them (so partially written .del files, etc,
            // are removed)
            deleter->refresh();
        }
        finally.throwException();

        // Have the deleter remove any now unreferenced files due to this commit
        deleter->checkpoint(segmentInfos, true);
        deleter->close();

        maxIndexVersion = segmentInfos->getVersion();

        if (writeLock) {
            writeLock->release(); // release write lock
            writeLock.reset();
        }
    }
    _hasChanges = false;
}

void DirectoryReader::startCommit() {
    rollbackHasChanges = _hasChanges;
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        (*reader)->startCommit();
    }
}

void DirectoryReader::rollbackCommit() {
    _hasChanges = rollbackHasChanges;
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        (*reader)->rollbackCommit();
    }
}

MapStringString DirectoryReader::getCommitUserData() {
    ensureOpen();
    return segmentInfos->getUserData();
}

bool DirectoryReader::isCurrent() {
    ensureOpen();
    IndexWriterPtr writer(_writer.lock());
    if (!writer || writer->isClosed()) {
        // we loaded SegmentInfos from the directory
        return (SegmentInfos::readCurrentVersion(_directory) == segmentInfos->getVersion());
    } else {
        return writer->nrtIsCurrent(segmentInfosStart);
    }
}

void DirectoryReader::doClose() {
    SyncLock syncLock(this);
    LuceneException ioe;
    normsCache.reset();
    for (Collection<SegmentReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        // try to close each reader, even if an exception is thrown
        try {
            (*reader)->decRef();
        } catch (LuceneException& e) {
            if (ioe.isNull()) {
                ioe = e;
            }
        }
    }

    // NOTE: only needed in case someone had asked for FieldCache for top-level reader (which is
    // generally not a good idea):
    FieldCache::DEFAULT()->purge(shared_from_this());

    // throw the first exception
    ioe.throwException();
}

HashSet<String> DirectoryReader::getFieldNames(FieldOption fieldOption) {
    ensureOpen();
    return getFieldNames(fieldOption, Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end()));
}

HashSet<String> DirectoryReader::getFieldNames(FieldOption fieldOption, Collection<IndexReaderPtr> subReaders) {
    // maintain a unique set of field names
    HashSet<String> fieldSet(HashSet<String>::newInstance());

    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        HashSet<String> names((*reader)->getFieldNames(fieldOption));
        fieldSet.addAll(names.begin(), names.end());
    }
    return fieldSet;
}

Collection<IndexReaderPtr> DirectoryReader::getSequentialSubReaders() {
    return Collection<IndexReaderPtr>::newInstance(subReaders.begin(), subReaders.end());
}

DirectoryPtr DirectoryReader::directory() {
    // Don't ensureOpen here -- in certain cases, when a cloned/reopened reader needs to commit, it may call
    // this method on the closed original reader
    return _directory;
}

int32_t DirectoryReader::getTermInfosIndexDivisor() {
    return termInfosIndexDivisor;
}

IndexCommitPtr DirectoryReader::getIndexCommit() {
    return newLucene<ReaderCommit>(segmentInfos, _directory);
}

Collection<IndexCommitPtr> DirectoryReader::listCommits(const DirectoryPtr& dir) {
    HashSet<String> files(dir->listAll());

    Collection<IndexCommitPtr> commits(Collection<IndexCommitPtr>::newInstance());

    SegmentInfosPtr latest(newLucene<SegmentInfos>());
    latest->read(dir);
    int64_t currentGen = latest->getGeneration();

    commits.add(newLucene<ReaderCommit>(latest, dir));

    for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
        if (boost::starts_with(*fileName, IndexFileNames::SEGMENTS()) &&
                *fileName != IndexFileNames::SEGMENTS_GEN() &&
                SegmentInfos::generationFromSegmentsFileName(*fileName) < currentGen) {
            SegmentInfosPtr sis(newLucene<SegmentInfos>());
            try {
                // IOException allowed to throw there, in case segments_N is corrupt
                sis->read(dir, *fileName);
            } catch (FileNotFoundException&) {
                sis.reset();
            }

            if (sis) {
                commits.add(newLucene<ReaderCommit>(sis, dir));
            }
        }
    }

    return commits;
}

FindSegmentsOpen::FindSegmentsOpen(bool readOnly, const IndexDeletionPolicyPtr& deletionPolicy, int32_t termInfosIndexDivisor, const SegmentInfosPtr& infos, const DirectoryPtr& directory) : FindSegmentsFileT<IndexReaderPtr>(infos, directory) {
    this->readOnly = readOnly;
    this->deletionPolicy = deletionPolicy;
    this->termInfosIndexDivisor = termInfosIndexDivisor;
}

FindSegmentsOpen::~FindSegmentsOpen() {
}

IndexReaderPtr FindSegmentsOpen::doBody(const String& segmentFileName) {
    SegmentInfosPtr segmentInfos(_segmentInfos);
    segmentInfos->read(directory, segmentFileName);
    if (readOnly) {
        return newLucene<ReadOnlyDirectoryReader>(directory, segmentInfos, deletionPolicy, termInfosIndexDivisor);
    } else {
        return newLucene<DirectoryReader>(directory, segmentInfos, deletionPolicy, false, termInfosIndexDivisor);
    }
}

FindSegmentsReopen::FindSegmentsReopen(const DirectoryReaderPtr& reader, bool openReadOnly, const SegmentInfosPtr& infos, const DirectoryPtr& directory) : FindSegmentsFileT<DirectoryReaderPtr>(infos, directory) {
    this->_reader = reader;
    this->openReadOnly = openReadOnly;
}

FindSegmentsReopen::~FindSegmentsReopen() {
}

DirectoryReaderPtr FindSegmentsReopen::doBody(const String& segmentFileName) {
    SegmentInfosPtr segmentInfos(_segmentInfos);
    segmentInfos->read(directory, segmentFileName);
    return DirectoryReaderPtr(_reader)->doReopen(segmentInfos, false, openReadOnly);
}

MultiTermEnum::MultiTermEnum(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> readers, Collection<int32_t> starts, const TermPtr& t) {
    _docFreq = 0;
    this->_topReader = topReader;
    queue = newLucene<SegmentMergeQueue>(readers.size());
    matchingSegments = Collection<SegmentMergeInfoPtr>::newInstance(readers.size() + 1);
    for (int32_t i = 0; i < readers.size(); ++i) {
        IndexReaderPtr reader(readers[i]);
        TermEnumPtr termEnum;

        if (t) {
            termEnum = reader->terms(t);
        } else {
            termEnum = reader->terms();
        }

        SegmentMergeInfoPtr smi(newLucene<SegmentMergeInfo>(starts[i], termEnum, reader));
        smi->ord = i;
        if (t.get() != NULL ? termEnum->term().get() != NULL : smi->next()) {
            queue->add(smi);    // initialize queue
        } else {
            smi->close();
        }
    }

    if (t && !queue->empty()) {
        next();
    }
}

MultiTermEnum::~MultiTermEnum() {
}

bool MultiTermEnum::next() {
    for (Collection<SegmentMergeInfoPtr>::iterator smi = matchingSegments.begin(); smi != matchingSegments.end(); ++smi) {
        if (!(*smi)) {
            break;
        }
        if ((*smi)->next()) {
            queue->add(*smi);
        } else {
            (*smi)->close();    // done with segment
        }
    }

    int32_t numMatchingSegments = 0;
    matchingSegments[0].reset();

    SegmentMergeInfoPtr top(queue->top());

    if (!top) {
        _term.reset();
        return false;
    }

    _term = top->term;
    _docFreq = 0;

    while (top && _term->compareTo(top->term) == 0) {
        matchingSegments[numMatchingSegments++] = top;
        queue->pop();
        _docFreq += top->termEnum->docFreq(); // increment freq
        top = queue->top();
    }

    matchingSegments[numMatchingSegments].reset();
    return true;
}

TermPtr MultiTermEnum::term() {
    return _term;
}

int32_t MultiTermEnum::docFreq() {
    return _docFreq;
}

void MultiTermEnum::close() {
    queue->close();
}

MultiTermDocs::MultiTermDocs(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> r, Collection<int32_t> s) {
    this->_topReader = topReader;
    readers = r;
    starts = s;
    base = 0;
    pointer = 0;
    readerTermDocs = Collection<TermDocsPtr>::newInstance(r.size());
}

MultiTermDocs::~MultiTermDocs() {
}

int32_t MultiTermDocs::doc() {
    return base + current->doc();
}

int32_t MultiTermDocs::freq() {
    return current->freq();
}

void MultiTermDocs::seek(const TermPtr& term) {
    this->term = term;
    this->base = 0;
    this->pointer = 0;
    this->current.reset();
    this->tenum.reset();
    this->smi.reset();
    this->matchingSegmentPos = 0;
}

void MultiTermDocs::seek(const TermEnumPtr& termEnum) {
    seek(termEnum->term());
    MultiTermEnumPtr multiTermEnum(boost::dynamic_pointer_cast<MultiTermEnum>(termEnum));
    if (multiTermEnum) {
        tenum = multiTermEnum;
        if (IndexReaderPtr(_topReader) != IndexReaderPtr(tenum->_topReader)) {
            tenum.reset();
        }
    }
}

bool MultiTermDocs::next() {
    while (true) {
        if (current && current->next()) {
            return true;
        } else if (pointer < readers.size()) {
            if (tenum) {
                smi = tenum->matchingSegments[matchingSegmentPos++];
                if (!smi) {
                    pointer = readers.size();
                    return false;
                }
                pointer = smi->ord;
            }
            base = starts[pointer];
            current = termDocs(pointer++);
        } else {
            return false;
        }
    }
}

int32_t MultiTermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    while (true) {
        while (!current) {
            if (pointer < readers.size()) { // try next segment
                if (tenum) {
                    smi = tenum->matchingSegments[matchingSegmentPos++];
                    if (!smi) {
                        pointer = readers.size();
                        return 0;
                    }
                    pointer = smi->ord;
                }
                base = starts[pointer];
                current = termDocs(pointer++);
            } else {
                return 0;
            }
        }
        int32_t end = current->read(docs, freqs);
        if (end == 0) { // none left in segment
            current.reset();
        } else { // got some
            for (int32_t i = 0; i < end; ++i) { // adjust doc numbers
                docs[i] += base;
            }
            return end;
        }
    }
}

bool MultiTermDocs::skipTo(int32_t target) {
    while (true) {
        if (current && current->skipTo(target - base)) {
            return true;
        } else if (pointer < readers.size()) {
            if (tenum) {
                smi = tenum->matchingSegments[matchingSegmentPos++];
                if (!smi) {
                    pointer = readers.size();
                    return false;
                }
                pointer = smi->ord;
            }
            base = starts[pointer];
            current = termDocs(pointer++);
        } else {
            return false;
        }
    }
}

TermDocsPtr MultiTermDocs::termDocs(int32_t i) {
    TermDocsPtr result(readerTermDocs[i]);
    if (!result) {
        readerTermDocs[i] = termDocs(readers[i]);
        result = readerTermDocs[i];
    }
    if (smi) {
        BOOST_ASSERT(smi->ord == i);
        BOOST_ASSERT(smi->termEnum->term()->equals(term));
        result->seek(smi->termEnum);
    } else {
        result->seek(term);
    }
    return result;
}

TermDocsPtr MultiTermDocs::termDocs(const IndexReaderPtr& reader) {
    return term ? reader->termDocs() : reader->termDocs(TermPtr());
}

void MultiTermDocs::close() {
    for (Collection<TermDocsPtr>::iterator termDoc = readerTermDocs.begin(); termDoc != readerTermDocs.end(); ++termDoc) {
        if (*termDoc) {
            (*termDoc)->close();
        }
    }
}

MultiTermPositions::MultiTermPositions(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> r, Collection<int32_t> s) : MultiTermDocs(topReader, r, s) {
}

MultiTermPositions::~MultiTermPositions() {
}

TermDocsPtr MultiTermPositions::termDocs(const IndexReaderPtr& reader) {
    return reader->termPositions();
}

int32_t MultiTermPositions::nextPosition() {
    return boost::static_pointer_cast<TermPositions>(current)->nextPosition();
}

int32_t MultiTermPositions::getPayloadLength() {
    return boost::static_pointer_cast<TermPositions>(current)->getPayloadLength();
}

ByteArray MultiTermPositions::getPayload(ByteArray data, int32_t offset) {
    return boost::static_pointer_cast<TermPositions>(current)->getPayload(data, offset);
}

bool MultiTermPositions::isPayloadAvailable() {
    return boost::static_pointer_cast<TermPositions>(current)->isPayloadAvailable();
}

ReaderCommit::ReaderCommit(const SegmentInfosPtr& infos, const DirectoryPtr& dir) {
    segmentsFileName = infos->getCurrentSegmentFileName();
    this->dir = dir;
    userData = infos->getUserData();
    HashSet<String> files(infos->files(dir, true));
    this->files = HashSet<String>::newInstance(files.begin(), files.end());
    version = infos->getVersion();
    generation = infos->getGeneration();
    _isOptimized = infos->size() == 1 && !infos->info(0)->hasDeletions();
}

ReaderCommit::~ReaderCommit() {
}

String ReaderCommit::toString() {
    return L"DirectoryReader::ReaderCommit(" + segmentsFileName + L")";
}

bool ReaderCommit::isOptimized() {
    return _isOptimized;
}

String ReaderCommit::getSegmentsFileName() {
    return segmentsFileName;
}

HashSet<String> ReaderCommit::getFileNames() {
    return files;
}

DirectoryPtr ReaderCommit::getDirectory() {
    return dir;
}

int64_t ReaderCommit::getVersion() {
    return version;
}

int64_t ReaderCommit::getGeneration() {
    return generation;
}

bool ReaderCommit::isDeleted() {
    return false;
}

MapStringString ReaderCommit::getUserData() {
    return userData;
}

void ReaderCommit::deleteCommit() {
    boost::throw_exception(UnsupportedOperationException(L"This IndexCommit does not support deletions."));
}

}
