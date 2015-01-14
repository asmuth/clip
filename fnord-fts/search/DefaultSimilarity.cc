/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/DefaultSimilarity.h"
#include "fnord-fts/index/FieldInvertState.h"

namespace fnord {
namespace fts {

DefaultSimilarity::DefaultSimilarity() {
    discountOverlaps = false;
}

DefaultSimilarity::~DefaultSimilarity() {
}

double DefaultSimilarity::computeNorm(const String& fieldName, const FieldInvertStatePtr& state) {
    int32_t numTerms;
    if (discountOverlaps) {
        numTerms = state->getLength() - state->getNumOverlap();
    } else {
        numTerms = state->getLength();
    }
    return (state->getBoost() * lengthNorm(fieldName, numTerms));
}

double DefaultSimilarity::lengthNorm(const String& fieldName, int32_t numTokens) {
    return (double)(1.0 / std::sqrt((double)numTokens));
}

double DefaultSimilarity::queryNorm(double sumOfSquaredWeights) {
    return (double)(1.0 / std::sqrt(sumOfSquaredWeights));
}

double DefaultSimilarity::tf(double freq) {
    return (double)std::sqrt(freq);
}

double DefaultSimilarity::sloppyFreq(int32_t distance) {
    return (1.0 / (double)(distance + 1));
}

double DefaultSimilarity::idf(int32_t docFreq, int32_t numDocs) {
    return (double)(std::log((double)numDocs / (double)(docFreq + 1)) + 1.0);
}

double DefaultSimilarity::coord(int32_t overlap, int32_t maxOverlap) {
    return (double)overlap / (double)maxOverlap;
}

void DefaultSimilarity::setDiscountOverlaps(bool v) {
    discountOverlaps = v;
}

bool DefaultSimilarity::getDiscountOverlaps() {
    return discountOverlaps;
}

}

}
