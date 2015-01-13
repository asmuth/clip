/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ParallelReader.h"
#include "_ParallelReader.h"
#include "Document.h"
#include "FieldSelector.h"
#include "Term.h"
#include "FieldCache.h"
#include "StringUtils.h"

namespace Lucene {

ParallelReader::ParallelReader(bool closeSubReaders) {
    this->readers = Collection<IndexReaderPtr>::newInstance();
    this->decrefOnClose = Collection<uint8_t>::newInstance();
    this->fieldToReader = MapStringIndexReader::newInstance();
    this->readerToFields = MapIndexReaderSetString::newInstance();
    this->storedFieldReaders = Collection<IndexReaderPtr>::newInstance();
    this->_maxDoc = 0;
    this->_numDocs = 0;
    this->_hasDeletions = false;

    this->incRefReaders = !closeSubReaders;
}

ParallelReader::~ParallelReader() {
}

void ParallelReader::add(const IndexReaderPtr& reader) {
    ensureOpen();
    add(reader, false);
}

void ParallelReader::add(const IndexReaderPtr& reader, bool ignoreStoredFields) {
    ensureOpen();
    if (readers.empty()) {
        this->_maxDoc = reader->maxDoc();
        this->_numDocs = reader->numDocs();
        this->_hasDeletions = reader->hasDeletions();
    }

    if (reader->maxDoc() != _maxDoc) { // check compatibility
        boost::throw_exception(IllegalArgumentException(L"All readers must have same maxDoc: " + StringUtils::toString(_maxDoc) +
                               L" != " + StringUtils::toString(reader->maxDoc())));
    }
    if (reader->numDocs() != _numDocs) {
        boost::throw_exception(IllegalArgumentException(L"All readers must have same numDocs: " + StringUtils::toString(_numDocs) +
                               L" != " + StringUtils::toString(reader->numDocs())));
    }

    HashSet<String> fields(reader->getFieldNames(IndexReader::FIELD_OPTION_ALL));
    readerToFields.put(reader, fields);
    for (HashSet<String>::iterator field = fields.begin(); field != fields.end(); ++field) { // update fieldToReader map
        if (!fieldToReader.contains(*field)) {
            fieldToReader.put(*field, reader);
        }
    }

    if (!ignoreStoredFields) {
        storedFieldReaders.add(reader);    // add to storedFieldReaders
    }
    readers.add(reader);

    if (incRefReaders) {
        reader->incRef();
    }

    decrefOnClose.add(incRefReaders);
}

LuceneObjectPtr ParallelReader::clone(const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    try {
        return doReopen(true);
    } catch (LuceneException& e) {
        boost::throw_exception(RuntimeException(e.getError()));
    }
    return LuceneObjectPtr();
}

IndexReaderPtr ParallelReader::reopen() {
    SyncLock syncLock(this);
    return doReopen(false);
}

IndexReaderPtr ParallelReader::doReopen(bool doClone) {
    ensureOpen();

    bool reopened = false;
    Collection<IndexReaderPtr> newReaders(Collection<IndexReaderPtr>::newInstance());

    bool success = false;
    LuceneException finally;
    try {
        for (Collection<IndexReaderPtr>::iterator oldReader = readers.begin(); oldReader != readers.end(); ++oldReader) {
            IndexReaderPtr newReader;
            if (doClone) {
                newReader = boost::dynamic_pointer_cast<IndexReader>((*oldReader)->clone());
            } else {
                newReader = (*oldReader)->reopen();
            }
            newReaders.add(newReader);
            // if at least one of the subreaders was updated we remember that and return a new ParallelReader
            if (newReader != *oldReader) {
                reopened = true;
            }
        }
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success && reopened) {
        for (int32_t i = 0; i < newReaders.size(); ++i) {
            if (newReaders[i] != readers[i]) {
                try {
                    if (newReaders[i]) {
                        newReaders[i]->close();
                    }
                } catch (...) {
                    // keep going - we want to clean up as much as possible
                }
            }
        }
    }
    finally.throwException();

    if (reopened) {
        Collection<uint8_t> newDecrefOnClose(Collection<uint8_t>::newInstance());
        ParallelReaderPtr pr(newLucene<ParallelReader>());
        for (int32_t i = 0; i < readers.size(); ++i) {
            IndexReaderPtr oldReader(readers[i]);
            IndexReaderPtr newReader(newReaders[i]);
            if (newReader == oldReader) {
                newDecrefOnClose.add(true);
                newReader->incRef();
            } else {
                // this is a new subreader instance, so on close() we don't decRef but close it
                newDecrefOnClose.add(false);
            }
            pr->add(newReader, !storedFieldReaders.contains(oldReader));
        }
        pr->decrefOnClose = newDecrefOnClose;
        pr->incRefReaders = incRefReaders;
        return pr;
    } else {
        // No subreader was refreshed
        return shared_from_this();
    }
}

int32_t ParallelReader::numDocs() {
    // Don't call ensureOpen() here (it could affect performance)
    return _numDocs;
}

int32_t ParallelReader::maxDoc() {
    // Don't call ensureOpen() here (it could affect performance)
    return _maxDoc;
}

bool ParallelReader::hasDeletions() {
    // Don't call ensureOpen() here (it could affect performance)
    return _hasDeletions;
}

bool ParallelReader::isDeleted(int32_t n) {
    // Don't call ensureOpen() here (it could affect performance)
    return !readers.empty() ? readers[0]->isDeleted(n) : false; // check first reader
}

void ParallelReader::doDelete(int32_t docNum) {
    // delete in all readers
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        (*reader)->deleteDocument(docNum);
    }
    _hasDeletions = true;
}

void ParallelReader::doUndeleteAll() {
    // undeleteAll in all readers
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        (*reader)->undeleteAll();
    }
    _hasDeletions = false;
}

DocumentPtr ParallelReader::document(int32_t n, const FieldSelectorPtr& fieldSelector) {
    ensureOpen();
    DocumentPtr result(newLucene<Document>());

    // append fields from storedFieldReaders
    for (Collection<IndexReaderPtr>::iterator reader = storedFieldReaders.begin(); reader != storedFieldReaders.end(); ++reader) {
        bool include = !fieldSelector;
        if (!include) {
            HashSet<String> fields = readerToFields.get(*reader);
            for (HashSet<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
                if (fieldSelector->accept(*field) != FieldSelector::SELECTOR_NO_LOAD) {
                    include = true;
                    break;
                }
            }
        }
        if (include) {
            Collection<FieldablePtr> fields((*reader)->document(n, fieldSelector)->getFields());
            for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
                result->add(*field);
            }
        }
    }
    return result;
}

Collection<TermFreqVectorPtr> ParallelReader::getTermFreqVectors(int32_t docNumber) {
    ensureOpen();

    Collection<TermFreqVectorPtr> results(Collection<TermFreqVectorPtr>::newInstance());

    // get all vectors
    for (MapStringIndexReader::iterator entry = fieldToReader.begin(); entry != fieldToReader.end(); ++entry) {
        TermFreqVectorPtr vector(entry->second->getTermFreqVector(docNumber, entry->first));
        if (vector) {
            results.add(vector);
        }
    }

    return results;
}

TermFreqVectorPtr ParallelReader::getTermFreqVector(int32_t docNumber, const String& field) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    return reader == fieldToReader.end() ? TermFreqVectorPtr() : reader->second->getTermFreqVector(docNumber, field);
}

void ParallelReader::getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    if (reader != fieldToReader.end()) {
        reader->second->getTermFreqVector(docNumber, field, mapper);
    }
}

void ParallelReader::getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    for (MapStringIndexReader::iterator entry = fieldToReader.begin(); entry != fieldToReader.end(); ++entry) {
        entry->second->getTermFreqVector(docNumber, entry->first, mapper);
    }
}

bool ParallelReader::hasNorms(const String& field) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    return reader == fieldToReader.end() ? false : reader->second->hasNorms(field);
}

ByteArray ParallelReader::norms(const String& field) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    return reader == fieldToReader.end() ? ByteArray() : reader->second->norms(field);
}

void ParallelReader::norms(const String& field, ByteArray norms, int32_t offset) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    if (reader != fieldToReader.end()) {
        reader->second->norms(field, norms, offset);
    }
}

void ParallelReader::doSetNorm(int32_t doc, const String& field, uint8_t value) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(field);
    if (reader != fieldToReader.end()) {
        reader->second->doSetNorm(doc, field, value);
    }
}

TermEnumPtr ParallelReader::terms() {
    ensureOpen();
    return newLucene<ParallelTermEnum>(shared_from_this());
}

TermEnumPtr ParallelReader::terms(const TermPtr& t) {
    ensureOpen();
    return newLucene<ParallelTermEnum>(shared_from_this(), t);
}

int32_t ParallelReader::docFreq(const TermPtr& t) {
    ensureOpen();
    MapStringIndexReader::iterator reader = fieldToReader.find(t->field());
    return reader == fieldToReader.end() ? 0 : reader->second->docFreq(t);
}

TermDocsPtr ParallelReader::termDocs(const TermPtr& term) {
    ensureOpen();
    return newLucene<ParallelTermDocs>(shared_from_this(), term);
}

TermDocsPtr ParallelReader::termDocs() {
    ensureOpen();
    return newLucene<ParallelTermDocs>(shared_from_this());
}

TermPositionsPtr ParallelReader::termPositions(const TermPtr& term) {
    ensureOpen();
    return newLucene<ParallelTermPositions>(shared_from_this(), term);
}

TermPositionsPtr ParallelReader::termPositions() {
    ensureOpen();
    return newLucene<ParallelTermPositions>(shared_from_this());
}

bool ParallelReader::isCurrent() {
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        if (!(*reader)->isCurrent()) {
            return false;
        }
    }

    // all subreaders are up to date
    return true;
}

bool ParallelReader::isOptimized() {
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        if (!(*reader)->isOptimized()) {
            return false;
        }
    }

    // all subindexes are optimized
    return true;
}

int64_t ParallelReader::getVersion() {
    boost::throw_exception(UnsupportedOperationException(L"ParallelReader does not support this method."));
    return 0;
}

Collection<IndexReaderPtr> ParallelReader::getSubReaders() {
    return readers;
}

void ParallelReader::doCommit(MapStringString commitUserData) {
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        (*reader)->commit(commitUserData);
    }
}

void ParallelReader::doClose() {
    SyncLock syncLock(this);
    for (int32_t i = 0; i < readers.size(); ++i) {
        if (decrefOnClose[i]) {
            readers[i]->decRef();
        } else {
            readers[i]->close();
        }
    }

    FieldCache::DEFAULT()->purge(shared_from_this());
}

HashSet<String> ParallelReader::getFieldNames(FieldOption fieldOption) {
    ensureOpen();
    HashSet<String> fieldSet(HashSet<String>::newInstance());
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        HashSet<String> names((*reader)->getFieldNames(fieldOption));
        fieldSet.addAll(names.begin(), names.end());
    }
    return fieldSet;
}

ParallelTermEnum::ParallelTermEnum(const ParallelReaderPtr& reader) {
    this->setIterator = false;
    this->_reader = reader;
    MapStringIndexReader::iterator indexReader = reader->fieldToReader.begin();
    if (indexReader != reader->fieldToReader.end()) {
        this->field = indexReader->first;
    }
    if (!field.empty()) {
        this->termEnum = reader->fieldToReader[field]->terms();
    }
}

ParallelTermEnum::ParallelTermEnum(const ParallelReaderPtr& reader, const TermPtr& term) {
    this->setIterator = false;
    this->_reader = reader;
    this->field = term->field();
    MapStringIndexReader::iterator indexReader = reader->fieldToReader.find(field);
    if (indexReader != reader->fieldToReader.end()) {
        this->termEnum = indexReader->second->terms(term);
    }
}

ParallelTermEnum::~ParallelTermEnum() {
}

bool ParallelTermEnum::next() {
    if (!termEnum) {
        return false;
    }

    // another term in this field?
    if (termEnum->next() && termEnum->term()->field() == field) {
        return true;    // yes, keep going
    }

    termEnum->close(); // close old termEnum
    ParallelReaderPtr reader(_reader);

    // find the next field with terms, if any
    if (!setIterator) {
        fieldIterator = reader->fieldToReader.find(field);
        ++fieldIterator; // Skip field to get next one
        setIterator = false;
    }

    while (fieldIterator != reader->fieldToReader.end()) {
        field = fieldIterator->first;
        termEnum = fieldIterator->second->terms(newLucene<Term>(field));
        ++fieldIterator;
        TermPtr term(termEnum->term());
        if (term && term->field() == field) {
            return true;
        } else {
            termEnum->close();
        }
    }

    return false; // no more fields
}

TermPtr ParallelTermEnum::term() {
    return termEnum ? termEnum->term() : TermPtr();
}

int32_t ParallelTermEnum::docFreq() {
    return termEnum ? termEnum->docFreq() : 0;
}

void ParallelTermEnum::close() {
    if (termEnum) {
        termEnum->close();
    }
}

ParallelTermDocs::ParallelTermDocs(const ParallelReaderPtr& reader) {
    this->_reader = reader;
}

ParallelTermDocs::ParallelTermDocs(const ParallelReaderPtr& reader, const TermPtr& term) {
    this->_reader = reader;
    if (!term) {
        termDocs = reader->readers.empty() ? TermDocsPtr() : reader->readers[0]->termDocs(TermPtr());
    } else {
        seek(term);
    }
}

ParallelTermDocs::~ParallelTermDocs() {
}

int32_t ParallelTermDocs::doc() {
    return termDocs->doc();
}

int32_t ParallelTermDocs::freq() {
    return termDocs->freq();
}

void ParallelTermDocs::seek(const TermPtr& term) {
    ParallelReaderPtr reader(_reader);
    MapStringIndexReader::iterator indexReader = reader->fieldToReader.find(term->field());
    termDocs = indexReader != reader->fieldToReader.end() ? indexReader->second->termDocs(term) : TermDocsPtr();
}

void ParallelTermDocs::seek(const TermEnumPtr& termEnum) {
    seek(termEnum->term());
}

bool ParallelTermDocs::next() {
    return termDocs ? termDocs->next() : false;
}

int32_t ParallelTermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    return termDocs ? termDocs->read(docs, freqs) : 0;
}

bool ParallelTermDocs::skipTo(int32_t target) {
    return termDocs ? termDocs->skipTo(target) : false;
}

void ParallelTermDocs::close() {
    if (termDocs) {
        termDocs->close();
    }
}

ParallelTermPositions::ParallelTermPositions(const ParallelReaderPtr& reader) : ParallelTermDocs(reader) {
}

ParallelTermPositions::ParallelTermPositions(const ParallelReaderPtr& reader, const TermPtr& term) : ParallelTermDocs(reader) {
    seek(term);
}

ParallelTermPositions::~ParallelTermPositions() {
}

void ParallelTermPositions::seek(const TermPtr& term) {
    ParallelReaderPtr reader(_reader);
    MapStringIndexReader::iterator indexReader = reader->fieldToReader.find(term->field());
    termDocs = indexReader != reader->fieldToReader.end() ? indexReader->second->termPositions(term) : TermDocsPtr();
}

int32_t ParallelTermPositions::nextPosition() {
    // It is an error to call this if there is no next position, eg. if termDocs==null
    return boost::static_pointer_cast<TermPositions>(termDocs)->nextPosition();
}

int32_t ParallelTermPositions::getPayloadLength() {
    // It is an error to call this if there is no next position, eg. if termDocs==null
    return boost::static_pointer_cast<TermPositions>(termDocs)->getPayloadLength();
}

ByteArray ParallelTermPositions::getPayload(ByteArray data, int32_t offset) {
    // It is an error to call this if there is no next position, eg. if termDocs==null
    return boost::static_pointer_cast<TermPositions>(termDocs)->getPayload(data, offset);
}

bool ParallelTermPositions::isPayloadAvailable() {
    // It is an error to call this if there is no next position, eg. if termDocs==null
    return boost::static_pointer_cast<TermPositions>(termDocs)->isPayloadAvailable();
}

}
