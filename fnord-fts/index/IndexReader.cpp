/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include "IndexReader.h"
#include "_IndexReader.h"
#include "DirectoryReader.h"
#include "IndexDeletionPolicy.h"
#include "FSDirectory.h"
#include "FieldSelector.h"
#include "Similarity.h"
#include "CompoundFileReader.h"
#include "FileUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t IndexReader::DEFAULT_TERMS_INDEX_DIVISOR = 1;

IndexReader::IndexReader() {
    refCount = 1;
    closed = false;
    _hasChanges = false;
}

IndexReader::~IndexReader() {
}

int32_t IndexReader::getRefCount() {
    SyncLock syncLock(this);
    return refCount;
}

void IndexReader::incRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0);
    ensureOpen();
    ++refCount;
}

void IndexReader::decRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0);
    ensureOpen();
    if (refCount == 1) {
        commit();
        doClose();
    }
    --refCount;
}

void IndexReader::ensureOpen() {
    if (refCount <= 0) {
        boost::throw_exception(AlreadyClosedException(L"this IndexReader is closed"));
    }
}

IndexReaderPtr IndexReader::open(const DirectoryPtr& directory) {
    return open(directory, IndexDeletionPolicyPtr(), IndexCommitPtr(), true, DEFAULT_TERMS_INDEX_DIVISOR);
}

IndexReaderPtr IndexReader::open(const DirectoryPtr& directory, bool readOnly) {
    return open(directory, IndexDeletionPolicyPtr(), IndexCommitPtr(), readOnly, DEFAULT_TERMS_INDEX_DIVISOR);
}

IndexReaderPtr IndexReader::open(const IndexCommitPtr& commit, bool readOnly) {
    return open(commit->getDirectory(), IndexDeletionPolicyPtr(), commit, readOnly, DEFAULT_TERMS_INDEX_DIVISOR);
}

IndexReaderPtr IndexReader::open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly) {
    return open(directory, deletionPolicy, IndexCommitPtr(), readOnly, DEFAULT_TERMS_INDEX_DIVISOR);
}

IndexReaderPtr IndexReader::open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor) {
    return open(directory, deletionPolicy, IndexCommitPtr(), readOnly, termInfosIndexDivisor);
}

IndexReaderPtr IndexReader::open(const IndexCommitPtr& commit, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly) {
    return open(commit->getDirectory(), deletionPolicy, commit, readOnly, DEFAULT_TERMS_INDEX_DIVISOR);
}

IndexReaderPtr IndexReader::open(const IndexCommitPtr& commit, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor) {
    return open(commit->getDirectory(), deletionPolicy, commit, readOnly, termInfosIndexDivisor);
}

IndexReaderPtr IndexReader::open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, const IndexCommitPtr& commit, bool readOnly, int32_t termInfosIndexDivisor) {
    return DirectoryReader::open(directory, deletionPolicy, commit, readOnly, termInfosIndexDivisor);
}

IndexReaderPtr IndexReader::reopen() {
    SyncLock syncLock(this);
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support reopen()."));
    return IndexReaderPtr();
}

IndexReaderPtr IndexReader::reopen(bool openReadOnly) {
    SyncLock syncLock(this);
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support reopen()."));
    return IndexReaderPtr();
}

IndexReaderPtr IndexReader::reopen(const IndexCommitPtr& commit) {
    SyncLock syncLock(this);
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support reopen(IndexCommit)."));
    return IndexReaderPtr();
}

LuceneObjectPtr IndexReader::clone(const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    if (!other) {
        boost::throw_exception(UnsupportedOperationException(L"This reader does not implement clone()."));
    }
    return other;
}

LuceneObjectPtr IndexReader::clone(bool openReadOnly, const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    if (!other) {
        boost::throw_exception(UnsupportedOperationException(L"This reader does not implement clone(bool)."));
    }
    return other;
}

DirectoryPtr IndexReader::directory() {
    ensureOpen();
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return DirectoryPtr();
}

int64_t IndexReader::lastModified(const DirectoryPtr& directory2) {
    return newLucene<FindSegmentsModified>(newLucene<SegmentInfos>(), directory2)->run();
}

int64_t IndexReader::getCurrentVersion(const DirectoryPtr& directory) {
    return SegmentInfos::readCurrentVersion(directory);
}

MapStringString IndexReader::getCommitUserData(const DirectoryPtr& directory) {
    return SegmentInfos::readCurrentUserData(directory);
}

int64_t IndexReader::getVersion() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return 0;
}

MapStringString IndexReader::getCommitUserData() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return MapStringString();
}

bool IndexReader::isCurrent() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return false;
}

bool IndexReader::isOptimized() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return false;
}

bool IndexReader::indexExists(const DirectoryPtr& directory) {
    return (SegmentInfos::getCurrentSegmentGeneration(directory) != -1);
}

int32_t IndexReader::numDeletedDocs() {
    return (maxDoc() - numDocs());
}

DocumentPtr IndexReader::document(int32_t n) {
    ensureOpen();
    return document(n, FieldSelectorPtr());
}

bool IndexReader::hasChanges() {
    return _hasChanges;
}

bool IndexReader::hasNorms(const String& field) {
    // backward compatible implementation.
    // SegmentReader has an efficient implementation.
    ensureOpen();
    return norms(field);
}

void IndexReader::setNorm(int32_t doc, const String& field, uint8_t value) {
    SyncLock syncLock(this);
    ensureOpen();
    acquireWriteLock();
    _hasChanges = true;
    doSetNorm(doc, field, value);
}

void IndexReader::setNorm(int32_t doc, const String& field, double value) {
    ensureOpen();
    setNorm(doc, field, Similarity::encodeNorm(value));
}

TermDocsPtr IndexReader::termDocs(const TermPtr& term) {
    ensureOpen();
    TermDocsPtr _termDocs(termDocs());
    _termDocs->seek(term);
    return _termDocs;
}

TermPositionsPtr IndexReader::termPositions(const TermPtr& term) {
    ensureOpen();
    TermPositionsPtr _termPositions(termPositions());
    _termPositions->seek(term);
    return _termPositions;
}

void IndexReader::deleteDocument(int32_t docNum) {
    SyncLock syncLock(this);
    ensureOpen();
    acquireWriteLock();
    _hasChanges = true;
    doDelete(docNum);
}

int32_t IndexReader::deleteDocuments(const TermPtr& term) {
    ensureOpen();
    TermDocsPtr docs(termDocs(term));
    if (!docs) {
        return 0;
    }
    int32_t n = 0;
    LuceneException finally;
    try {
        while (docs->next()) {
            deleteDocument(docs->doc());
            ++n;
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    docs->close();
    finally.throwException();
    return n;
}

void IndexReader::undeleteAll() {
    SyncLock syncLock(this);
    ensureOpen();
    acquireWriteLock();
    _hasChanges = true;
    doUndeleteAll();
}

void IndexReader::acquireWriteLock() {
    SyncLock syncLock(this);
    // NOOP
}

void IndexReader::flush() {
    SyncLock syncLock(this);
    ensureOpen();
    commit();
}

void IndexReader::flush(MapStringString commitUserData) {
    SyncLock syncLock(this);
    ensureOpen();
    commit(commitUserData);
}

void IndexReader::commit() {
    commit(MapStringString());
}

void IndexReader::commit(MapStringString commitUserData) {
    SyncLock syncLock(this);
    if (_hasChanges) {
        doCommit(commitUserData);
    }
    _hasChanges = false;
}

void IndexReader::close() {
    SyncLock syncLock(this);
    if (!closed) {
        decRef();
        closed = true;
    }
}

IndexCommitPtr IndexReader::getIndexCommit() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return IndexCommitPtr();
}

void IndexReader::main(Collection<String> args) {
    String filename;
    bool extract = false;

    for (Collection<String>::iterator arg = args.begin(); arg != args.end(); ++arg) {
        if (*arg == L"-extract") {
            extract = true;
        } else if (filename.empty()) {
            filename = *arg;
        }
    }

    if (filename.empty()) {
        std::wcout << L"Usage: IndexReader [-extract] <cfsfile>";
        return;
    }

    DirectoryPtr dir;
    CompoundFileReaderPtr cfr;

    LuceneException finally;
    try {
        String dirname(FileUtils::extractPath(filename));
        filename = FileUtils::extractPath(filename);
        dir = FSDirectory::open(dirname);
        cfr = newLucene<CompoundFileReader>(dir, filename);

        HashSet<String> _files(cfr->listAll());
        Collection<String> files(Collection<String>::newInstance(_files.begin(), _files.end()));
        std::sort(files.begin(), files.end()); // sort the array of filename so that the output is more readable

        for (Collection<String>::iterator file = files.begin(); file != files.end(); ++file) {
            int64_t len = cfr->fileLength(*file);

            if (extract) {
                std::wcout << L"extract " << *file << L" with " << len << L" bytes to local directory...";
                IndexInputPtr ii(cfr->openInput(*file));

                boost::filesystem::ofstream f(*file, std::ios::binary | std::ios::out);

                // read and write with a small buffer, which is more effective than reading byte by byte
                ByteArray buffer(ByteArray::newInstance(1024));

                int32_t chunk = buffer.size();
                while (len > 0) {
                    int32_t bufLen = std::min(chunk, (int32_t)len);
                    ii->readBytes(buffer.get(), 0, bufLen);
                    f.write((char*)buffer.get(), bufLen);
                    len -= bufLen;
                }
                ii->close();
            } else {
                std::wcout << *file << L": " << len << " bytes\n";
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }

    if (dir) {
        dir->close();
    }
    if (cfr) {
        cfr->close();
    }

    finally.throwException();
}

Collection<IndexCommitPtr> IndexReader::listCommits(const DirectoryPtr& dir) {
    return DirectoryReader::listCommits(dir);
}

Collection<IndexReaderPtr> IndexReader::getSequentialSubReaders() {
    return Collection<IndexReaderPtr>(); // override
}

LuceneObjectPtr IndexReader::getFieldCacheKey() {
    return shared_from_this();
}

LuceneObjectPtr IndexReader::getDeletesCacheKey() {
    return shared_from_this();
}

int64_t IndexReader::getUniqueTermCount() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not implement getUniqueTermCount()"));
    return 0;
}

int32_t IndexReader::getTermInfosIndexDivisor() {
    boost::throw_exception(UnsupportedOperationException(L"This reader does not support this method."));
    return 0;
}

FindSegmentsModified::FindSegmentsModified(const SegmentInfosPtr& infos, const DirectoryPtr& directory) : FindSegmentsFileT<uint64_t>(infos, directory) {
    result = 0;
}

FindSegmentsModified::~FindSegmentsModified() {
}

uint64_t FindSegmentsModified::doBody(const String& segmentFileName) {
    return directory->fileModified(segmentFileName);
}

}
