/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultiReader.h"
#include "DirectoryReader.h"
#include "DefaultSimilarity.h"
#include "FieldCache.h"
#include "MiscUtils.h"

namespace Lucene {

MultiReader::MultiReader(Collection<IndexReaderPtr> subReaders, bool closeSubReaders) {
    this->normsCache = MapStringByteArray::newInstance();
    this->_maxDoc = 0;
    this->_numDocs = -1;
    this->_hasDeletions = false;
    this->subReaders = subReaders;
    starts = Collection<int32_t>::newInstance(subReaders.size() + 1); // build starts array
    decrefOnClose = Collection<uint8_t>::newInstance(subReaders.size());
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        starts[i] = _maxDoc;
        _maxDoc += subReaders[i]->maxDoc(); // compute maxDocs

        if (!closeSubReaders) {
            subReaders[i]->incRef();
            decrefOnClose[i] = true;
        } else {
            decrefOnClose[i] = false;
        }

        if (subReaders[i]->hasDeletions()) {
            _hasDeletions = true;
        }
    }
    starts[subReaders.size()] = _maxDoc;
}

MultiReader::~MultiReader() {
}

IndexReaderPtr MultiReader::reopen() {
    SyncLock syncLock(this);
    return doReopen(false);
}

LuceneObjectPtr MultiReader::clone(const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    try {
        return doReopen(true);
    } catch (LuceneException& e) {
        boost::throw_exception(RuntimeException(e.getError()));
    }
    return LuceneObjectPtr();
}

IndexReaderPtr MultiReader::doReopen(bool doClone) {
    ensureOpen();

    bool reopened = false;
    Collection<IndexReaderPtr> newSubReaders(Collection<IndexReaderPtr>::newInstance(subReaders.size()));

    bool success = false;
    LuceneException finally;
    try {
        for (int32_t i = 0; i < subReaders.size(); ++i) {
            if (doClone) {
                newSubReaders[i] = boost::dynamic_pointer_cast<IndexReader>(subReaders[i]->clone());
            } else {
                newSubReaders[i] = subReaders[i]->reopen();
            }
            // if at least one of the subreaders was updated we remember that and return a new MultiReader
            if (newSubReaders[i] != subReaders[i]) {
                reopened = true;
            }
        }
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success && reopened) {
        for (int32_t i = 0; i < newSubReaders.size(); ++i) {
            if (newSubReaders[i] != subReaders[i]) {
                try {
                    if (newSubReaders[i]) {
                        newSubReaders[i]->close();
                    }
                } catch (...) {
                    // keep going - we want to clean up as much as possible
                }
            }
        }
    }
    finally.throwException();

    if (reopened) {
        Collection<uint8_t> newDecrefOnClose(Collection<uint8_t>::newInstance(subReaders.size()));
        for (int32_t i = 0; i < subReaders.size(); ++i) {
            if (newSubReaders[i] == subReaders[i]) {
                newSubReaders[i]->incRef();
                newDecrefOnClose[i] = true;
            }
        }

        MultiReaderPtr mr(newLucene<MultiReader>(newSubReaders));
        mr->decrefOnClose = newDecrefOnClose;
        return mr;
    } else {
        return shared_from_this();
    }
}

Collection<TermFreqVectorPtr> MultiReader::getTermFreqVectors(int32_t docNumber) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    return subReaders[i]->getTermFreqVectors(docNumber - starts[i]); // dispatch to segment
}

TermFreqVectorPtr MultiReader::getTermFreqVector(int32_t docNumber, const String& field) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    return subReaders[i]->getTermFreqVector(docNumber - starts[i], field);
}

void MultiReader::getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    subReaders[i]->getTermFreqVector(docNumber - starts[i], field, mapper);
}

void MultiReader::getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    int32_t i = readerIndex(docNumber); // find segment num
    subReaders[i]->getTermFreqVector(docNumber - starts[i], mapper);
}

bool MultiReader::isOptimized() {
    return false;
}

int32_t MultiReader::numDocs() {
    // Don't call ensureOpen() here (it could affect performance)

    // NOTE: multiple threads may wind up init'ing numDocs... but that's harmless
    if (_numDocs == -1) {
        // check cache
        int32_t n = 0; // cache miss - recompute
        for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
            n += (*reader)->numDocs();    // sum from readers
        }
        _numDocs = n;
    }
    return _numDocs;
}

int32_t MultiReader::maxDoc() {
    // Don't call ensureOpen() here (it could affect performance)
    return _maxDoc;
}

DocumentPtr MultiReader::document(int32_t n, const FieldSelectorPtr& fieldSelector) {
    ensureOpen();
    int32_t i = readerIndex(n); // find segment num
    return subReaders[i]->document(n - starts[i], fieldSelector); // dispatch to segment reader
}

bool MultiReader::isDeleted(int32_t n) {
    // Don't call ensureOpen() here (it could affect performance)
    int32_t i = readerIndex(n); // find segment num
    return subReaders[i]->isDeleted(n - starts[i]); // dispatch to segment reader
}

bool MultiReader::hasDeletions() {
    // Don't call ensureOpen() here (it could affect performance)
    return _hasDeletions;
}

void MultiReader::doDelete(int32_t docNum) {
    _numDocs = -1; // invalidate cache
    int32_t i = readerIndex(docNum); // find segment num
    subReaders[i]->deleteDocument(docNum - starts[i]); // dispatch to segment reader
    _hasDeletions = true;
}

void MultiReader::doUndeleteAll() {
    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        (*reader)->undeleteAll();
    }
    _hasDeletions = false;
    _numDocs = -1; // invalidate cache
}

int32_t MultiReader::readerIndex(int32_t n) {
    return DirectoryReader::readerIndex(n, this->starts, this->subReaders.size());
}

bool MultiReader::hasNorms(const String& field) {
    ensureOpen();
    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        if ((*reader)->hasNorms(field)) {
            return true;
        }
    }
    return false;
}

ByteArray MultiReader::norms(const String& field) {
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

void MultiReader::norms(const String& field, ByteArray norms, int32_t offset) {
    SyncLock syncLock(this);
    ensureOpen();
    ByteArray bytes(normsCache.get(field));
    for (int32_t i = 0; i < subReaders.size(); ++i) { // read from segments
        subReaders[i]->norms(field, norms, offset + starts[i]);
    }

    if (!bytes && !hasNorms(field)) {
        MiscUtils::arrayFill(norms.get(), offset, norms.size(), DefaultSimilarity::encodeNorm(1.0));
    } else if (bytes) { // cache hit
        MiscUtils::arrayCopy(bytes.get(), 0, norms.get(), offset, maxDoc());
    } else {
        for (int32_t i = 0; i < subReaders.size(); ++i) {
            subReaders[i]->norms(field, norms, offset + starts[i]);
        }
    }
}

void MultiReader::doSetNorm(int32_t doc, const String& field, uint8_t value) {
    {
        SyncLock normsLock(&normsCache);
        normsCache.remove(field); // clear cache
    }
    int32_t i = readerIndex(doc); // find segment num
    subReaders[i]->setNorm(doc - starts[i], field, value); // dispatch
}

TermEnumPtr MultiReader::terms() {
    ensureOpen();
    return newLucene<MultiTermEnum>(shared_from_this(), subReaders, starts, TermPtr());
}

TermEnumPtr MultiReader::terms(const TermPtr& t) {
    ensureOpen();
    return newLucene<MultiTermEnum>(shared_from_this(), subReaders, starts, t);
}

int32_t MultiReader::docFreq(const TermPtr& t) {
    ensureOpen();
    int32_t total = 0; // sum freqs in segments
    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        total += (*reader)->docFreq(t);
    }
    return total;
}

TermDocsPtr MultiReader::termDocs() {
    ensureOpen();
    return newLucene<MultiTermDocs>(shared_from_this(), subReaders, starts);
}

TermPositionsPtr MultiReader::termPositions() {
    ensureOpen();
    return newLucene<MultiTermPositions>(shared_from_this(), subReaders, starts);
}

void MultiReader::doCommit(MapStringString commitUserData) {
    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        (*reader)->commit(commitUserData);
    }
}

void MultiReader::doClose() {
    SyncLock syncLock(this);
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        if (decrefOnClose[i]) {
            subReaders[i]->decRef();
        } else {
            subReaders[i]->close();
        }
    }

    // NOTE: only needed in case someone had asked for FieldCache for top-level reader (which is
    // generally not a good idea)
    FieldCache::DEFAULT()->purge(shared_from_this());
}

HashSet<String> MultiReader::getFieldNames(FieldOption fieldOption) {
    ensureOpen();
    return DirectoryReader::getFieldNames(fieldOption, this->subReaders);
}

bool MultiReader::isCurrent() {
    for (Collection<IndexReaderPtr>::iterator reader = subReaders.begin(); reader != subReaders.end(); ++reader) {
        if (!(*reader)->isCurrent()) {
            return false;
        }
    }
    // all subreaders are up to date
    return true;
}

int64_t MultiReader::getVersion() {
    boost::throw_exception(UnsupportedOperationException());
    return 0;
}

Collection<IndexReaderPtr> MultiReader::getSequentialSubReaders() {
    return subReaders;
}

}
