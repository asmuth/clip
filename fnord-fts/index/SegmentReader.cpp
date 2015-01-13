/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "SegmentReader.h"
#include "_SegmentReader.h"
#include "IndexFileNames.h"
#include "DirectoryReader.h"
#include "CompoundFileReader.h"
#include "FieldInfos.h"
#include "FieldInfo.h"
#include "FieldsReader.h"
#include "TermInfo.h"
#include "TermInfosReader.h"
#include "TermVectorsReader.h"
#include "IndexOutput.h"
#include "ReadOnlySegmentReader.h"
#include "BitVector.h"
#include "SegmentTermEnum.h"
#include "SegmentTermPositions.h"
#include "SegmentInfo.h"
#include "SegmentMerger.h"
#include "AllTermDocs.h"
#include "DefaultSimilarity.h"
#include "FieldCache.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

SegmentReader::SegmentReader() {
    _norms = MapStringNorm::newInstance();
    readOnly = false;
    deletedDocsDirty = false;
    normsDirty = false;
    rollbackHasChanges = false;
    rollbackDeletedDocsDirty = false;
    rollbackNormsDirty = false;

    readBufferSize = 0;
    pendingDeleteCount = 0;
    rollbackPendingDeleteCount = 0;
}

SegmentReader::~SegmentReader() {
}

void SegmentReader::initialize() {
    fieldsReaderLocal = newLucene<FieldsReaderLocal>(shared_from_this());
}

SegmentReaderPtr SegmentReader::get(bool readOnly, const SegmentInfoPtr& si, int32_t termInfosIndexDivisor) {
    return get(readOnly, si->dir, si, BufferedIndexInput::BUFFER_SIZE, true, termInfosIndexDivisor);
}

SegmentReaderPtr SegmentReader::get(bool readOnly, const DirectoryPtr& dir, const SegmentInfoPtr& si, int32_t readBufferSize, bool doOpenStores, int32_t termInfosIndexDivisor) {
    SegmentReaderPtr instance(readOnly ? newLucene<ReadOnlySegmentReader>() : newLucene<SegmentReader>());
    instance->readOnly = readOnly;
    instance->si = si;
    instance->readBufferSize = readBufferSize;

    bool success = false;
    LuceneException finally;
    try {
        instance->core = newLucene<CoreReaders>(instance, dir, si, readBufferSize, termInfosIndexDivisor);
        if (doOpenStores) {
            instance->core->openDocStores(si);
        }
        instance->loadDeletedDocs();
        instance->openNorms(instance->core->cfsDir, readBufferSize);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    // With lock-less commits, it's entirely possible (and fine) to hit a FileNotFound exception above.
    // In this case, we want to explicitly close any subset of things that were opened
    if (!success) {
        instance->doClose();
    }
    finally.throwException();
    return instance;
}

void SegmentReader::openDocStores() {
    core->openDocStores(si);
}

bool SegmentReader::checkDeletedCounts() {
    int32_t recomputedCount = deletedDocs->getRecomputedCount();

    BOOST_ASSERT(deletedDocs->count() == recomputedCount);

    BOOST_ASSERT(si->getDelCount() == recomputedCount);

    // Verify # deletes does not exceed maxDoc for this segment
    BOOST_ASSERT(si->getDelCount() <= maxDoc());

    return true;
}

void SegmentReader::loadDeletedDocs() {
    // NOTE: the bitvector is stored using the regular directory, not cfs
    if (hasDeletions(si)) {
        deletedDocs = newLucene<BitVector>(directory(), si->getDelFileName());
        deletedDocsRef = newLucene<SegmentReaderRef>();
        BOOST_ASSERT(checkDeletedCounts());
    } else {
        BOOST_ASSERT(si->getDelCount() == 0);
    }
}

ByteArray SegmentReader::cloneNormBytes(ByteArray bytes) {
    ByteArray cloneBytes(ByteArray::newInstance(bytes.size()));
    MiscUtils::arrayCopy(bytes.get(), 0, cloneBytes.get(), 0, bytes.size());
    return cloneBytes;
}

BitVectorPtr SegmentReader::cloneDeletedDocs(const BitVectorPtr& bv) {
    return boost::dynamic_pointer_cast<BitVector>(bv->clone());
}

LuceneObjectPtr SegmentReader::clone(const LuceneObjectPtr& other) {
    try {
        return SegmentReader::clone(readOnly, other); // Preserve current readOnly
    } catch (...) {
        boost::throw_exception(RuntimeException());
    }
    return LuceneObjectPtr();
}

LuceneObjectPtr SegmentReader::clone(bool openReadOnly, const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    return reopenSegment(si, true, openReadOnly);
}

SegmentReaderPtr SegmentReader::reopenSegment(const SegmentInfoPtr& si, bool doClone, bool openReadOnly) {
    SyncLock syncLock(this);

    bool deletionsUpToDate = (this->si->hasDeletions() == si->hasDeletions() &&
                              (!si->hasDeletions() || this->si->getDelFileName() == si->getDelFileName()));
    bool normsUpToDate = true;
    int32_t fieldCount = core->fieldInfos->size();
    Collection<uint8_t> fieldNormsChanged(Collection<uint8_t>::newInstance(fieldCount));

    for (int32_t i = 0; i < fieldCount; ++i) {
        if (this->si->getNormFileName(i) != si->getNormFileName(i)) {
            normsUpToDate = false;
            fieldNormsChanged[i] = true;
        }
    }

    // if we're cloning we need to run through the reopenSegment logic also if both old and new readers
    // aren't readonly, we clone to avoid sharing modifications
    if (normsUpToDate && deletionsUpToDate && !doClone && openReadOnly && readOnly) {
        return shared_from_this();
    }

    // When cloning, the incoming SegmentInfos should not have any changes in it
    BOOST_ASSERT(!doClone || (normsUpToDate && deletionsUpToDate));

    // clone reader
    SegmentReaderPtr clone(openReadOnly ? newLucene<ReadOnlySegmentReader>() : newLucene<SegmentReader>());

    bool success = false;
    LuceneException finally;
    try {
        core->incRef();
        clone->core = core;
        clone->readOnly = openReadOnly;
        clone->si = si;
        clone->readBufferSize = readBufferSize;

        if (!openReadOnly && _hasChanges) {
            // My pending changes transfer to the new reader
            clone->pendingDeleteCount = pendingDeleteCount;
            clone->deletedDocsDirty = deletedDocsDirty;
            clone->normsDirty = normsDirty;
            clone->_hasChanges = _hasChanges;
            _hasChanges = false;
        }

        if (doClone) {
            if (deletedDocs) {
                deletedDocsRef->incRef();
                clone->deletedDocs = deletedDocs;
                clone->deletedDocsRef = deletedDocsRef;
            }
        } else {
            if (!deletionsUpToDate) {
                // load deleted docs
                BOOST_ASSERT(!clone->deletedDocs);
                clone->loadDeletedDocs();
            } else if (deletedDocs) {
                deletedDocsRef->incRef();
                clone->deletedDocs = deletedDocs;
                clone->deletedDocsRef = deletedDocsRef;
            }
        }

        clone->_norms = MapStringNorm::newInstance();

        // Clone norms
        for (int32_t i = 0; i < fieldNormsChanged.size(); ++i) {
            // Clone unchanged norms to the cloned reader
            if (doClone || !fieldNormsChanged[i]) {
                String curField(core->fieldInfos->fieldInfo(i)->name);
                NormPtr norm(this->_norms.get(curField));
                if (norm) {
                    NormPtr cloneNorm(boost::dynamic_pointer_cast<Norm>(norm->clone()));
                    cloneNorm->_reader = clone;
                    clone->_norms.put(curField, cloneNorm);
                }
            }
        }

        // If we are not cloning, then this will open anew any norms that have changed
        clone->openNorms(si->getUseCompoundFile() ? core->getCFSReader() : directory(), readBufferSize);

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        // An exception occurred during reopen, we have to decRef the norms that we incRef'ed already
        // and close singleNormsStream and FieldsReader
        clone->decRef();
    }
    finally.throwException();
    return clone;
}

void SegmentReader::doCommit(MapStringString commitUserData) {
    if (_hasChanges) {
        startCommit();
        bool success = false;
        LuceneException finally;
        try {
            commitChanges(commitUserData);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            rollbackCommit();
        }
        finally.throwException();
    }
}

void SegmentReader::commitChanges(MapStringString commitUserData) {
    if (deletedDocsDirty) { // re-write deleted
        si->advanceDelGen();

        // We can write directly to the actual name (vs to a .tmp & renaming it) because the file
        // is not live until segments file is written
        String delFileName(si->getDelFileName());

        bool success = false;
        LuceneException finally;
        try {
            deletedDocs->write(directory(), delFileName);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            try {
                directory()->deleteFile(delFileName);
            } catch (...) {
                // suppress this so we keep throwing the original exception
            }
        }
        finally.throwException();

        si->setDelCount(si->getDelCount() + pendingDeleteCount);
        pendingDeleteCount = 0;
        BOOST_ASSERT(deletedDocs->count() == si->getDelCount()); // delete count mismatch during commit?
    } else {
        BOOST_ASSERT(pendingDeleteCount == 0);
    }

    if (normsDirty) { // re-write norms
        si->setNumFields(core->fieldInfos->size());
        for (MapStringNorm::iterator norm = _norms.begin(); norm != _norms.end(); ++norm) {
            if (norm->second->dirty) {
                norm->second->reWrite(si);
            }
        }
    }
    deletedDocsDirty = false;
    normsDirty = false;
    _hasChanges = false;
}

FieldsReaderPtr SegmentReader::getFieldsReader() {
    return fieldsReaderLocal->get();
}

void SegmentReader::doClose() {
    termVectorsLocal.close();
    fieldsReaderLocal->close();
    if (deletedDocs) {
        deletedDocsRef->decRef();
        deletedDocs.reset(); // null so if an app hangs on to us we still free most ram
    }
    for (MapStringNorm::iterator norm = _norms.begin(); norm != _norms.end(); ++norm) {
        norm->second->decRef();
    }
    if (core) {
        core->decRef();
    }
}

bool SegmentReader::hasDeletions(const SegmentInfoPtr& si) {
    // Don't call ensureOpen() here (it could affect performance)
    return si->hasDeletions();
}

bool SegmentReader::hasDeletions() {
    // Don't call ensureOpen() here (it could affect performance)
    return deletedDocs.get() != NULL;
}

bool SegmentReader::usesCompoundFile(const SegmentInfoPtr& si) {
    return si->getUseCompoundFile();
}

bool SegmentReader::hasSeparateNorms(const SegmentInfoPtr& si) {
    return si->hasSeparateNorms();
}

void SegmentReader::doDelete(int32_t docNum) {
    if (!deletedDocs) {
        deletedDocs = newLucene<BitVector>(maxDoc());
        deletedDocsRef = newLucene<SegmentReaderRef>();
    }
    // there is more than 1 SegmentReader with a reference to this deletedDocs BitVector so decRef
    // the current deletedDocsRef, clone the BitVector, create a new deletedDocsRef
    if (deletedDocsRef->refCount() > 1) {
        SegmentReaderRefPtr oldRef(deletedDocsRef);
        deletedDocs = cloneDeletedDocs(deletedDocs);
        deletedDocsRef = newLucene<SegmentReaderRef>();
        oldRef->decRef();
    }
    deletedDocsDirty = true;
    if (!deletedDocs->getAndSet(docNum)) {
        ++pendingDeleteCount;
    }
}

void SegmentReader::doUndeleteAll() {
    deletedDocsDirty = false;
    if (deletedDocs) {
        BOOST_ASSERT(deletedDocsRef);
        deletedDocsRef->decRef();
        deletedDocs.reset();
        deletedDocsRef.reset();
        pendingDeleteCount = 0;
        si->clearDelGen();
        si->setDelCount(0);
    } else {
        BOOST_ASSERT(!deletedDocsRef);
        BOOST_ASSERT(pendingDeleteCount == 0);
    }
}

HashSet<String> SegmentReader::files() {
    return si->files();
}

TermEnumPtr SegmentReader::terms() {
    ensureOpen();
    return core->getTermsReader()->terms();
}

TermEnumPtr SegmentReader::terms(const TermPtr& t) {
    ensureOpen();
    return core->getTermsReader()->terms(t);
}

FieldInfosPtr SegmentReader::fieldInfos() {
    return core->fieldInfos;
}

DocumentPtr SegmentReader::document(int32_t n, const FieldSelectorPtr& fieldSelector) {
    ensureOpen();
    return getFieldsReader()->doc(n, fieldSelector);
}

bool SegmentReader::isDeleted(int32_t n) {
    SyncLock syncLock(this);
    return (deletedDocs && deletedDocs->get(n));
}

TermDocsPtr SegmentReader::termDocs(const TermPtr& term) {
    if (!term) {
        return newLucene<AllTermDocs>(shared_from_this());
    } else {
        return IndexReader::termDocs(term);
    }
}

TermDocsPtr SegmentReader::termDocs() {
    ensureOpen();
    return newLucene<SegmentTermDocs>(shared_from_this());
}

TermPositionsPtr SegmentReader::termPositions() {
    ensureOpen();
    return newLucene<SegmentTermPositions>(shared_from_this());
}

int32_t SegmentReader::docFreq(const TermPtr& t) {
    ensureOpen();
    TermInfoPtr ti(core->getTermsReader()->get(t));
    return ti ? ti->docFreq : 0;
}

int32_t SegmentReader::numDocs() {
    // Don't call ensureOpen() here (it could affect performance)
    int32_t n = maxDoc();
    if (deletedDocs) {
        n -= deletedDocs->count();
    }
    return n;
}

int32_t SegmentReader::maxDoc() {
    // Don't call ensureOpen() here (it could affect performance)
    return si->docCount;
}

HashSet<String> SegmentReader::getFieldNames(FieldOption fieldOption) {
    ensureOpen();
    HashSet<String> fieldSet(HashSet<String>::newInstance());
    for (int32_t i = 0; i < core->fieldInfos->size(); ++i) {
        FieldInfoPtr fi(core->fieldInfos->fieldInfo(i));
        if (fieldOption == FIELD_OPTION_ALL) {
            fieldSet.add(fi->name);
        } else if (!fi->isIndexed && fieldOption == FIELD_OPTION_UNINDEXED) {
            fieldSet.add(fi->name);
        } else if (fi->omitTermFreqAndPositions && fieldOption == FIELD_OPTION_OMIT_TERM_FREQ_AND_POSITIONS) {
            fieldSet.add(fi->name);
        } else if (fi->storePayloads && fieldOption == FIELD_OPTION_STORES_PAYLOADS) {
            fieldSet.add(fi->name);
        } else if (fi->isIndexed && fieldOption == FIELD_OPTION_INDEXED) {
            fieldSet.add(fi->name);
        } else if (fi->isIndexed && !fi->storeTermVector && fieldOption == FIELD_OPTION_INDEXED_NO_TERMVECTOR) {
            fieldSet.add(fi->name);
        } else if (fi->storeTermVector && !fi->storePositionWithTermVector && !fi->storeOffsetWithTermVector && fieldOption == FIELD_OPTION_TERMVECTOR) {
            fieldSet.add(fi->name);
        } else if (fi->isIndexed && fi->storeTermVector && fieldOption == FIELD_OPTION_INDEXED_WITH_TERMVECTOR) {
            fieldSet.add(fi->name);
        } else if (fi->storePositionWithTermVector && !fi->storeOffsetWithTermVector && fieldOption == FIELD_OPTION_TERMVECTOR_WITH_POSITION) {
            fieldSet.add(fi->name);
        } else if (fi->storeOffsetWithTermVector && !fi->storePositionWithTermVector && fieldOption == FIELD_OPTION_TERMVECTOR_WITH_OFFSET) {
            fieldSet.add(fi->name);
        } else if (fi->storeOffsetWithTermVector && fi->storePositionWithTermVector && fieldOption == FIELD_OPTION_TERMVECTOR_WITH_POSITION_OFFSET) {
            fieldSet.add(fi->name);
        }
    }
    return fieldSet;
}

bool SegmentReader::hasNorms(const String& field) {
    SyncLock syncLock(this);
    ensureOpen();
    return _norms.contains(field);
}

ByteArray SegmentReader::getNorms(const String& field) {
    SyncLock syncLock(this);
    NormPtr norm(_norms.get(field));
    return norm ? norm->bytes() : ByteArray();
}

ByteArray SegmentReader::norms(const String& field) {
    SyncLock syncLock(this);
    ensureOpen();
    return getNorms(field);
}

void SegmentReader::doSetNorm(int32_t doc, const String& field, uint8_t value) {
    NormPtr norm(_norms.get(field));
    if (!norm) { // not an indexed field
        return;
    }

    normsDirty = true;
    ByteArray bytes(norm->copyOnWrite());
    if (doc < 0 || doc >= bytes.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    bytes[doc] = value; // set the value
}

void SegmentReader::norms(const String& field, ByteArray norms, int32_t offset) {
    SyncLock syncLock(this);
    ensureOpen();
    NormPtr norm(_norms.get(field));
    if (!norm) {
        MiscUtils::arrayFill(norms.get(), offset, norms.size(), DefaultSimilarity::encodeNorm(1.0));
        return;
    }

    norm->bytes(norms.get(), offset, maxDoc());
}

void SegmentReader::openNorms(const DirectoryPtr& cfsDir, int32_t readBufferSize) {
    int64_t nextNormSeek = SegmentMerger::NORMS_HEADER_LENGTH; // skip header (header unused for now)
    int32_t _maxDoc = maxDoc();
    for (int32_t i = 0; i < core->fieldInfos->size(); ++i) {
        FieldInfoPtr fi(core->fieldInfos->fieldInfo(i));
        if (_norms.contains(fi->name)) {
            // in case this SegmentReader is being re-opened, we might be able to reuse some norm
            // instances and skip loading them here
            continue;
        }
        if (fi->isIndexed && !fi->omitNorms) {
            DirectoryPtr d(directory());
            String fileName(si->getNormFileName(fi->number));
            if (!si->hasSeparateNorms(fi->number)) {
                d = cfsDir;
            }

            // singleNormFile means multiple norms share this file
            bool singleNormFile = boost::ends_with(fileName, String(L".") + IndexFileNames::NORMS_EXTENSION());
            IndexInputPtr normInput;
            int64_t normSeek;

            if (singleNormFile) {
                normSeek = nextNormSeek;
                if (!singleNormStream) {
                    singleNormStream = d->openInput(fileName, readBufferSize);
                    singleNormRef = newLucene<SegmentReaderRef>();
                } else {
                    singleNormRef->incRef();
                }

                // All norms in the .nrm file can share a single IndexInput since they are only used in
                // a synchronized context.  If this were to change in the future, a clone could be done here.
                normInput = singleNormStream;
            } else {
                normSeek = 0;
                normInput = d->openInput(fileName);
            }

            _norms.put(fi->name, newLucene<Norm>(shared_from_this(), normInput, fi->number, normSeek));
            nextNormSeek += _maxDoc; // increment also if some norms are separate
        }
    }
}

bool SegmentReader::termsIndexLoaded() {
    return core->termsIndexIsLoaded();
}

void SegmentReader::loadTermsIndex(int32_t termsIndexDivisor) {
    core->loadTermsIndex(si, termsIndexDivisor);
}

bool SegmentReader::normsClosed() {
    if (singleNormStream) {
        return false;
    }
    for (MapStringNorm::iterator norm = _norms.begin(); norm != _norms.end(); ++norm) {
        if (norm->second->refCount > 0) {
            return false;
        }
    }
    return true;
}

bool SegmentReader::normsClosed(const String& field) {
    return (_norms.get(field)->refCount == 0);
}

TermVectorsReaderPtr SegmentReader::getTermVectorsReader() {
    TermVectorsReaderPtr tvReader(termVectorsLocal.get());
    if (!tvReader) {
        TermVectorsReaderPtr orig(core->getTermVectorsReaderOrig());
        if (!orig) {
            return TermVectorsReaderPtr();
        } else {
            try {
                tvReader = boost::dynamic_pointer_cast<TermVectorsReader>(orig->clone());
            } catch (...) {
                return TermVectorsReaderPtr();
            }
        }
        termVectorsLocal.set(tvReader);
    }
    return tvReader;
}

TermVectorsReaderPtr SegmentReader::getTermVectorsReaderOrig() {
    return core->getTermVectorsReaderOrig();
}

TermFreqVectorPtr SegmentReader::getTermFreqVector(int32_t docNumber, const String& field) {
    // Check if this field is invalid or has no stored term vector
    ensureOpen();
    FieldInfoPtr fi(core->fieldInfos->fieldInfo(field));
    if (!fi || !fi->storeTermVector) {
        return TermFreqVectorPtr();
    }

    TermVectorsReaderPtr termVectorsReader(getTermVectorsReader());
    if (!termVectorsReader) {
        return TermFreqVectorPtr();
    }

    return termVectorsReader->get(docNumber, field);
}

void SegmentReader::getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    FieldInfoPtr fi(core->fieldInfos->fieldInfo(field));
    if (!fi || !fi->storeTermVector) {
        return;
    }

    TermVectorsReaderPtr termVectorsReader(getTermVectorsReader());
    if (!termVectorsReader) {
        return;
    }

    termVectorsReader->get(docNumber, field, mapper);
}

void SegmentReader::getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    ensureOpen();

    TermVectorsReaderPtr termVectorsReader(getTermVectorsReader());
    if (!termVectorsReader) {
        return;
    }

    termVectorsReader->get(docNumber, mapper);
}

Collection<TermFreqVectorPtr> SegmentReader::getTermFreqVectors(int32_t docNumber) {
    ensureOpen();

    TermVectorsReaderPtr termVectorsReader(getTermVectorsReader());
    if (!termVectorsReader) {
        return Collection<TermFreqVectorPtr>();
    }

    return termVectorsReader->get(docNumber);
}

String SegmentReader::getSegmentName() {
    return core->segment;
}

SegmentInfoPtr SegmentReader::getSegmentInfo() {
    return si;
}

void SegmentReader::setSegmentInfo(const SegmentInfoPtr& info) {
    si = info;
}

void SegmentReader::startCommit() {
    rollbackSegmentInfo = boost::dynamic_pointer_cast<SegmentInfo>(si->clone());
    rollbackHasChanges = _hasChanges;
    rollbackDeletedDocsDirty = deletedDocsDirty;
    rollbackNormsDirty = normsDirty;
    rollbackPendingDeleteCount = pendingDeleteCount;
    for (MapStringNorm::iterator norm = _norms.begin(); norm != _norms.end(); ++norm) {
        norm->second->rollbackDirty = norm->second->dirty;
    }
}

void SegmentReader::rollbackCommit() {
    si->reset(rollbackSegmentInfo);
    _hasChanges = rollbackHasChanges;
    deletedDocsDirty = rollbackDeletedDocsDirty;
    normsDirty = rollbackNormsDirty;
    pendingDeleteCount = rollbackPendingDeleteCount;
    for (MapStringNorm::iterator norm = _norms.begin(); norm != _norms.end(); ++norm) {
        norm->second->dirty = norm->second->rollbackDirty;
    }
}

DirectoryPtr SegmentReader::directory() {
    // Don't ensureOpen here - in certain cases, when a cloned/reopened reader needs to commit,
    // it may call this method on the closed original reader
    return core->dir;
}

LuceneObjectPtr SegmentReader::getFieldCacheKey() {
    return core->freqStream;
}

LuceneObjectPtr SegmentReader::getDeletesCacheKey() {
    return deletedDocs;
}

int64_t SegmentReader::getUniqueTermCount() {
    return core->getTermsReader()->size();
}

SegmentReaderPtr SegmentReader::getOnlySegmentReader(const DirectoryPtr& dir) {
    return getOnlySegmentReader(IndexReader::open(dir, false));
}

SegmentReaderPtr SegmentReader::getOnlySegmentReader(const IndexReaderPtr& reader) {
    SegmentReaderPtr segmentReader(boost::dynamic_pointer_cast<SegmentReader>(reader));
    if (segmentReader) {
        return segmentReader;
    }

    DirectoryReaderPtr directoryReader(boost::dynamic_pointer_cast<DirectoryReader>(reader));
    if (directoryReader) {
        Collection<IndexReaderPtr> subReaders(directoryReader->getSequentialSubReaders());
        if (subReaders.size() != 1) {
            boost::throw_exception(IllegalArgumentException(L"reader has " + StringUtils::toString(subReaders.size()) + L" segments instead of exactly one"));
        }
        return boost::dynamic_pointer_cast<SegmentReader>(subReaders[0]);
    }

    boost::throw_exception(IllegalArgumentException(L"reader is not a SegmentReader or a single-segment DirectoryReader"));

    return SegmentReaderPtr();
}

int32_t SegmentReader::getTermInfosIndexDivisor() {
    return core->termsIndexDivisor;
}

CoreReaders::CoreReaders(const SegmentReaderPtr& origInstance, const DirectoryPtr& dir, const SegmentInfoPtr& si, int32_t readBufferSize, int32_t termsIndexDivisor) {
    ref = newLucene<SegmentReaderRef>();

    segment = si->name;
    this->readBufferSize = readBufferSize;
    this->dir = dir;

    bool success = false;
    LuceneException finally;
    try {
        DirectoryPtr dir0(dir);
        if (si->getUseCompoundFile()) {
            cfsReader = newLucene<CompoundFileReader>(dir, segment + L"." + IndexFileNames::COMPOUND_FILE_EXTENSION(), readBufferSize);
            dir0 = cfsReader;
        }
        cfsDir = dir0;

        fieldInfos = newLucene<FieldInfos>(cfsDir, segment + L"." + IndexFileNames::FIELD_INFOS_EXTENSION());

        this->termsIndexDivisor = termsIndexDivisor;
        TermInfosReaderPtr reader(newLucene<TermInfosReader>(cfsDir, segment, fieldInfos, readBufferSize, termsIndexDivisor));
        if (termsIndexDivisor == -1) {
            tisNoIndex = reader;
        } else {
            tis = reader;
        }

        // make sure that all index files have been read or are kept open so that if an index
        // update removes them we'll still have them
        freqStream = cfsDir->openInput(segment + L"." + IndexFileNames::FREQ_EXTENSION(), readBufferSize);

        if (fieldInfos->hasProx()) {
            proxStream = cfsDir->openInput(segment + L"." + IndexFileNames::PROX_EXTENSION(), readBufferSize);
        }

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        decRef();
    }
    finally.throwException();

    // Must assign this at the end -- if we hit an exception above core, we don't want to attempt to
    // purge the FieldCache (will hit NPE because core is not assigned yet).
    _origInstance = origInstance;
}

CoreReaders::~CoreReaders() {
}

TermVectorsReaderPtr CoreReaders::getTermVectorsReaderOrig() {
    SyncLock syncLock(this);
    return termVectorsReaderOrig;
}

FieldsReaderPtr CoreReaders::getFieldsReaderOrig() {
    SyncLock syncLock(this);
    return fieldsReaderOrig;
}

void CoreReaders::incRef() {
    SyncLock syncLock(this);
    ref->incRef();
}

DirectoryPtr CoreReaders::getCFSReader() {
    SyncLock syncLock(this);
    return cfsReader;
}

TermInfosReaderPtr CoreReaders::getTermsReader() {
    SyncLock syncLock(this);
    return tis ? tis : tisNoIndex;
}

bool CoreReaders::termsIndexIsLoaded() {
    SyncLock syncLock(this);
    return tis.get() != NULL;
}

void CoreReaders::loadTermsIndex(const SegmentInfoPtr& si, int32_t termsIndexDivisor) {
    SyncLock syncLock(this);
    if (!tis) {
        DirectoryPtr dir0;
        if (si->getUseCompoundFile()) {
            // In some cases, we were originally opened when CFS was not used, but then we are asked
            // to open the terms reader with index, the segment has switched to CFS
            if (!cfsReader) {
                cfsReader = newLucene<CompoundFileReader>(dir, segment + L"." + IndexFileNames::COMPOUND_FILE_EXTENSION(), readBufferSize);
            }

            dir0 = cfsReader;
        } else {
            dir0 = dir;
        }

        tis = newLucene<TermInfosReader>(dir0, segment, fieldInfos, readBufferSize, termsIndexDivisor);
    }
}

void CoreReaders::decRef() {
    SyncLock syncLock(this);
    if (ref->decRef() == 0) {
        // close everything, nothing is shared anymore with other readers
        if (tis) {
            tis->close();
            tis.reset(); // null so if an app hangs on to us we still free most ram
        }
        if (tisNoIndex) {
            tisNoIndex->close();
        }
        if (freqStream) {
            freqStream->close();
        }
        if (proxStream) {
            proxStream->close();
        }
        if (termVectorsReaderOrig) {
            termVectorsReaderOrig->close();
        }
        if (fieldsReaderOrig) {
            fieldsReaderOrig->close();
        }
        if (cfsReader) {
            cfsReader->close();
        }
        if (storeCFSReader) {
            storeCFSReader->close();
        }

        // Force FieldCache to evict our entries at this point
        SegmentReaderPtr origInstance(_origInstance.lock());
        if (origInstance) {
            FieldCache::DEFAULT()->purge(origInstance);
        }
    }
}

void CoreReaders::openDocStores(const SegmentInfoPtr& si) {
    SyncLock syncLock(this);
    BOOST_ASSERT(si->name == segment);

    if (!fieldsReaderOrig) {
        DirectoryPtr storeDir;
        if (si->getDocStoreOffset() != -1) {
            if (si->getDocStoreIsCompoundFile()) {
                BOOST_ASSERT(!storeCFSReader);
                storeCFSReader = newLucene<CompoundFileReader>(dir, si->getDocStoreSegment() + L"." + IndexFileNames::COMPOUND_FILE_STORE_EXTENSION(), readBufferSize);
                storeDir = storeCFSReader;
                BOOST_ASSERT(storeDir);
            } else {
                storeDir = dir;
                BOOST_ASSERT(storeDir);
            }
        } else if (si->getUseCompoundFile()) {
            // In some cases, we were originally opened when CFS was not used, but then we are asked to open doc
            // stores after the segment has switched to CFS
            if (!cfsReader) {
                cfsReader = newLucene<CompoundFileReader>(dir, segment + L"." + IndexFileNames::COMPOUND_FILE_EXTENSION(), readBufferSize);
            }
            storeDir = cfsReader;
            BOOST_ASSERT(storeDir);
        } else {
            storeDir = dir;
            BOOST_ASSERT(storeDir);
        }

        String storesSegment(si->getDocStoreOffset() != -1 ? si->getDocStoreSegment() : segment);

        fieldsReaderOrig = newLucene<FieldsReader>(storeDir, storesSegment, fieldInfos, readBufferSize, si->getDocStoreOffset(), si->docCount);

        // Verify two sources of "maxDoc" agree
        if (si->getDocStoreOffset() == -1 && fieldsReaderOrig->size() != si->docCount) {
            boost::throw_exception(CorruptIndexException(L"doc counts differ for segment " + segment +
                                   L": fieldsReader shows " + StringUtils::toString(fieldsReaderOrig->size()) +
                                   L" but segmentInfo shows " + StringUtils::toString(si->docCount)));
        }

        if (fieldInfos->hasVectors()) { // open term vector files only as needed
            termVectorsReaderOrig = newLucene<TermVectorsReader>(storeDir, storesSegment, fieldInfos, readBufferSize, si->getDocStoreOffset(), si->docCount);
        }
    }
}

FieldsReaderLocal::FieldsReaderLocal(const SegmentReaderPtr& reader) {
    this->_reader = reader;
}

FieldsReaderPtr FieldsReaderLocal::initialValue() {
    return boost::dynamic_pointer_cast<FieldsReader>(SegmentReaderPtr(_reader)->core->getFieldsReaderOrig()->clone());
}

SegmentReaderRef::SegmentReaderRef() {
    _refCount = 1;
}

SegmentReaderRef::~SegmentReaderRef() {
}

String SegmentReaderRef::toString() {
    StringStream buffer;
    buffer << L"refcount: " << _refCount;
    return buffer.str();
}

int32_t SegmentReaderRef::refCount() {
    SyncLock syncLock(this);
    return _refCount;
}

int32_t SegmentReaderRef::incRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(_refCount > 0);
    return ++_refCount;
}

int32_t SegmentReaderRef::decRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(_refCount > 0);
    return --_refCount;
}

Norm::Norm() {
    this->refCount = 1;
    this->normSeek = 0;
    this->dirty = false;
    this->rollbackDirty = false;
    this->number = 0;
}

Norm::Norm(const SegmentReaderPtr& reader, const IndexInputPtr& in, int32_t number, int64_t normSeek) {
    this->_reader = reader;
    this->refCount = 1;
    this->dirty = false;
    this->rollbackDirty = false;
    this->in = in;
    this->number = number;
    this->normSeek = normSeek;
}

Norm::~Norm() {
}

void Norm::incRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));
    ++refCount;
}

void Norm::closeInput() {
    SegmentReaderPtr reader(_reader.lock());
    if (in && reader) {
        if (in != reader->singleNormStream) {
            // It's private to us -- just close it
            in->close();
        } else {
            // We are sharing this with others -- decRef and maybe close the shared norm stream
            if (reader->singleNormRef->decRef() == 0) {
                reader->singleNormStream->close();
                reader->singleNormStream.reset();
            }
        }

        in.reset();
    }
}

void Norm::decRef() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));

    if (--refCount == 0) {
        if (origNorm) {
            origNorm->decRef();
            origNorm.reset();
        } else {
            closeInput();
        }

        if (origReader) {
            origReader.reset();
        }

        if (_bytes) {
            BOOST_ASSERT(_bytesRef);
            _bytesRef->decRef();
            _bytes.reset();
            _bytesRef.reset();
        } else {
            BOOST_ASSERT(!_bytesRef);
        }
    }
}

void Norm::bytes(uint8_t* bytesOut, int32_t offset, int32_t length) {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));
    if (_bytes) {
        // Already cached - copy from cache
        BOOST_ASSERT(length <= SegmentReaderPtr(_reader)->maxDoc());
        MiscUtils::arrayCopy(_bytes.get(), 0, bytesOut, offset, length);
    } else {
        // Not cached
        if (origNorm) {
            // Ask origNorm to load
            origNorm->bytes(bytesOut, offset, length);
        } else {
            // We are orig - read ourselves from disk
            SyncLock instancesLock(in);
            in->seek(normSeek);
            in->readBytes(bytesOut, offset, length, false);
        }
    }
}

ByteArray Norm::bytes() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));
    if (!_bytes) { // value not yet read
        BOOST_ASSERT(!_bytesRef);
        if (origNorm) {
            // Ask origNorm to load so that for a series of reopened readers we share a single read-only byte[]
            _bytes = origNorm->bytes();
            _bytesRef = origNorm->_bytesRef;
            _bytesRef->incRef();

            // Once we've loaded the bytes we no longer need origNorm
            origNorm->decRef();
            origNorm.reset();
            origReader.reset();
        } else {
            // We are the origNorm, so load the bytes for real ourself
            int32_t count = SegmentReaderPtr(_reader)->maxDoc();
            _bytes = ByteArray::newInstance(count);

            // Since we are orig, in must not be null
            BOOST_ASSERT(in);

            // Read from disk.
            {
                SyncLock instancesLock(in);
                in->seek(normSeek);
                in->readBytes(_bytes.get(), 0, count, false);
            }

            _bytesRef = newLucene<SegmentReaderRef>();
            closeInput();
        }
    }

    return _bytes;
}

SegmentReaderRefPtr Norm::bytesRef() {
    return _bytesRef;
}

ByteArray Norm::copyOnWrite() {
    SyncLock syncLock(this);
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));
    bytes();
    BOOST_ASSERT(_bytes);
    BOOST_ASSERT(_bytesRef);
    if (_bytesRef->refCount() > 1) {
        // I cannot be the origNorm for another norm instance if I'm being changed.
        // ie, only the "head Norm" can be changed
        BOOST_ASSERT(refCount == 1);
        SegmentReaderRefPtr oldRef(_bytesRef);
        _bytes = SegmentReaderPtr(_reader)->cloneNormBytes(_bytes);
        _bytesRef = newLucene<SegmentReaderRef>();
        oldRef->decRef();
    }
    dirty = true;
    return _bytes;
}

LuceneObjectPtr Norm::clone(const LuceneObjectPtr& other) {
    SyncLock syncLock(this);

    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));
    LuceneObjectPtr clone = other ? other : newLucene<Norm>();
    NormPtr cloneNorm(boost::dynamic_pointer_cast<Norm>(clone));
    cloneNorm->_reader = _reader;
    cloneNorm->origNorm = origNorm;
    cloneNorm->origReader = origReader;
    cloneNorm->normSeek = normSeek;
    cloneNorm->_bytesRef = _bytesRef;
    cloneNorm->_bytes = _bytes;
    cloneNorm->dirty = dirty;
    cloneNorm->number = number;
    cloneNorm->rollbackDirty = rollbackDirty;

    cloneNorm->refCount = 1;

    if (_bytes) {
        BOOST_ASSERT(_bytesRef);
        BOOST_ASSERT(!origNorm);

        // Clone holds a reference to my bytes
        cloneNorm->_bytesRef->incRef();
    } else {
        BOOST_ASSERT(!_bytesRef);
        if (!origNorm) {
            // I become the origNorm for the clone
            cloneNorm->origNorm = shared_from_this();
            cloneNorm->origReader = SegmentReaderPtr(_reader);
        }
        cloneNorm->origNorm->incRef();
    }

    // Only the origNorm will actually readBytes from in
    cloneNorm->in.reset();

    return cloneNorm;
}

void Norm::reWrite(const SegmentInfoPtr& si) {
    BOOST_ASSERT(refCount > 0 && (!origNorm || origNorm->refCount > 0));

    // NOTE: norms are re-written in regular directory, not cfs
    si->advanceNormGen(this->number);
    String normFileName(si->getNormFileName(this->number));
    SegmentReaderPtr reader(_reader);
    IndexOutputPtr out(reader->directory()->createOutput(normFileName));
    bool success = false;
    LuceneException finally;
    try {
        try {
            out->writeBytes(_bytes.get(), reader->maxDoc());
        } catch (LuceneException& e) {
            finally = e;
        }
        out->close();
        finally.throwException();
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        try {
            reader->directory()->deleteFile(normFileName);
        } catch (...) {
            // suppress this so we keep throwing the original exception
        }
    }
    finally.throwException();
    this->dirty = false;
}

}
