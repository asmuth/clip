/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FilterIndexReader.h"
#include "FieldCache.h"

namespace Lucene {

FilterIndexReader::FilterIndexReader(const IndexReaderPtr& in) {
    this->in = in;
}

FilterIndexReader::~FilterIndexReader() {
}

DirectoryPtr FilterIndexReader::directory() {
    return in->directory();
}

Collection<TermFreqVectorPtr> FilterIndexReader::getTermFreqVectors(int32_t docNumber) {
    ensureOpen();
    return in->getTermFreqVectors(docNumber);
}

TermFreqVectorPtr FilterIndexReader::getTermFreqVector(int32_t docNumber, const String& field) {
    ensureOpen();
    return in->getTermFreqVector(docNumber, field);
}

void FilterIndexReader::getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    in->getTermFreqVector(docNumber, field, mapper);
}

void FilterIndexReader::getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    ensureOpen();
    in->getTermFreqVector(docNumber, mapper);
}

int32_t FilterIndexReader::numDocs() {
    // Don't call ensureOpen() here (it could affect performance)
    return in->numDocs();
}

int32_t FilterIndexReader::maxDoc() {
    // Don't call ensureOpen() here (it could affect performance)
    return in->maxDoc();
}

DocumentPtr FilterIndexReader::document(int32_t n, const FieldSelectorPtr& fieldSelector) {
    ensureOpen();
    return in->document(n, fieldSelector);
}

bool FilterIndexReader::isDeleted(int32_t n) {
    // Don't call ensureOpen() here (it could affect performance)
    return in->isDeleted(n);
}

bool FilterIndexReader::hasDeletions() {
    // Don't call ensureOpen() here (it could affect performance)
    return in->hasDeletions();
}

void FilterIndexReader::doUndeleteAll() {
    in->undeleteAll();
}

bool FilterIndexReader::hasNorms(const String& field) {
    ensureOpen();
    return in->hasNorms(field);
}

ByteArray FilterIndexReader::norms(const String& field) {
    ensureOpen();
    return in->norms(field);
}

void FilterIndexReader::norms(const String& field, ByteArray norms, int32_t offset) {
    ensureOpen();
    in->norms(field, norms, offset);
}

void FilterIndexReader::doSetNorm(int32_t doc, const String& field, uint8_t value) {
    in->setNorm(doc, field, value);
}

TermEnumPtr FilterIndexReader::terms() {
    ensureOpen();
    return in->terms();
}

TermEnumPtr FilterIndexReader::terms(const TermPtr& t) {
    ensureOpen();
    return in->terms(t);
}

int32_t FilterIndexReader::docFreq(const TermPtr& t) {
    ensureOpen();
    return in->docFreq(t);
}

TermDocsPtr FilterIndexReader::termDocs() {
    ensureOpen();
    return in->termDocs();
}

TermDocsPtr FilterIndexReader::termDocs(const TermPtr& term) {
    ensureOpen();
    return in->termDocs(term);
}

TermPositionsPtr FilterIndexReader::termPositions() {
    ensureOpen();
    return in->termPositions();
}

void FilterIndexReader::doDelete(int32_t docNum) {
    in->deleteDocument(docNum);
}

void FilterIndexReader::doCommit(MapStringString commitUserData) {
    in->commit(commitUserData);
}

void FilterIndexReader::doClose() {
    in->close();

    // NOTE: only needed in case someone had asked for FieldCache for top-level reader (which is
    // generally not a good idea)
    FieldCache::DEFAULT()->purge(shared_from_this());
}

HashSet<String> FilterIndexReader::getFieldNames(FieldOption fieldOption) {
    ensureOpen();
    return in->getFieldNames(fieldOption);
}

int64_t FilterIndexReader::getVersion() {
    ensureOpen();
    return in->getVersion();
}

bool FilterIndexReader::isCurrent() {
    ensureOpen();
    return in->isCurrent();
}

bool FilterIndexReader::isOptimized() {
    ensureOpen();
    return in->isOptimized();
}

Collection<IndexReaderPtr> FilterIndexReader::getSequentialSubReaders() {
    return in->getSequentialSubReaders();
}

LuceneObjectPtr FilterIndexReader::getFieldCacheKey() {
    return in->getFieldCacheKey();
}

LuceneObjectPtr FilterIndexReader::getDeletesCacheKey() {
    return in->getDeletesCacheKey();
}

FilterTermDocs::FilterTermDocs(const TermDocsPtr& in) {
    this->in = in;
}

FilterTermDocs::~FilterTermDocs() {
}

void FilterTermDocs::seek(const TermPtr& term) {
    in->seek(term);
}

void FilterTermDocs::seek(const TermEnumPtr& termEnum) {
    in->seek(termEnum);
}

int32_t FilterTermDocs::doc() {
    return in->doc();
}

int32_t FilterTermDocs::freq() {
    return in->freq();
}

bool FilterTermDocs::next() {
    return in->next();
}

int32_t FilterTermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    return in->read(docs, freqs);
}

bool FilterTermDocs::skipTo(int32_t target) {
    return in->skipTo(target);
}

void FilterTermDocs::close() {
    in->close();
}

FilterTermPositions::FilterTermPositions(const TermPositionsPtr& in) : FilterTermDocs(in) {
}

FilterTermPositions::~FilterTermPositions() {
}

int32_t FilterTermPositions::nextPosition() {
    return boost::static_pointer_cast<TermPositions>(in)->nextPosition();
}

int32_t FilterTermPositions::getPayloadLength() {
    return boost::static_pointer_cast<TermPositions>(in)->getPayloadLength();
}

ByteArray FilterTermPositions::getPayload(ByteArray data, int32_t offset) {
    return boost::static_pointer_cast<TermPositions>(in)->getPayload(data, offset);
}

bool FilterTermPositions::isPayloadAvailable() {
    return boost::static_pointer_cast<TermPositions>(in)->isPayloadAvailable();
}

FilterTermEnum::FilterTermEnum(const TermEnumPtr& in) {
    this->in = in;
}

FilterTermEnum::~FilterTermEnum() {
}

bool FilterTermEnum::next() {
    return in->next();
}

TermPtr FilterTermEnum::term() {
    return in->term();
}

int32_t FilterTermEnum::docFreq() {
    return in->docFreq();
}

void FilterTermEnum::close() {
    in->close();
}

}
