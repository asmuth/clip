/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/SimilarityDelegator.h"

namespace Lucene {

SimilarityDelegator::SimilarityDelegator(const SimilarityPtr& delegee) {
    this->delegee = delegee;
}

SimilarityDelegator::~SimilarityDelegator() {
}

double SimilarityDelegator::computeNorm(const String& fieldName, const FieldInvertStatePtr& state) {
    return delegee->computeNorm(fieldName, state);
}

double SimilarityDelegator::lengthNorm(const String& fieldName, int32_t numTokens) {
    return delegee->lengthNorm(fieldName, numTokens);
}

double SimilarityDelegator::queryNorm(double sumOfSquaredWeights) {
    return delegee->queryNorm(sumOfSquaredWeights);
}

double SimilarityDelegator::tf(double freq) {
    return delegee->tf(freq);
}

double SimilarityDelegator::sloppyFreq(int32_t distance) {
    return delegee->sloppyFreq(distance);
}

double SimilarityDelegator::idf(int32_t docFreq, int32_t numDocs) {
    return delegee->idf(docFreq, numDocs);
}

double SimilarityDelegator::coord(int32_t overlap, int32_t maxOverlap) {
    return delegee->coord(overlap, maxOverlap);
}

double SimilarityDelegator::scorePayload(int32_t docId, const String& fieldName, int32_t start, int32_t end, ByteArray payload, int32_t offset, int32_t length) {
    return delegee->scorePayload(docId, fieldName, start, end, payload, offset, length);
}

}
