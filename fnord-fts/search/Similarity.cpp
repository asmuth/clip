/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/search/_Similarity.h"
#include "fnord-fts/search/DefaultSimilarity.h"
#include "fnord-fts/index/FieldInvertState.h"
#include "fnord-fts/search/Searcher.h"
#include "fnord-fts/search/Term.h"
#include "fnord-fts/search/SmallDouble.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

const int32_t Similarity::NO_DOC_ID_PROVIDED = -1;

Similarity::Similarity() {
}

Similarity::~Similarity() {
}

SimilarityPtr Similarity::getDefault() {
    static SimilarityPtr defaultImpl;
    if (!defaultImpl) {
        defaultImpl = newLucene<DefaultSimilarity>();
        CycleCheck::addStatic(defaultImpl);
    }
    return defaultImpl;
}

const Collection<double> Similarity::NORM_TABLE() {
    static Collection<double> _NORM_TABLE;
    if (!_NORM_TABLE) {
        _NORM_TABLE = Collection<double>::newInstance(256);
        for (int32_t i = 0; i < 256; ++i) {
            _NORM_TABLE[i] = SmallDouble::byteToDouble((uint8_t)i);
        }
    }
    return _NORM_TABLE;
}

double Similarity::decodeNorm(uint8_t b) {
    return NORM_TABLE()[b & 0xff];  // & 0xff maps negative bytes to positive above 127
}

const Collection<double> Similarity::getNormDecoder() {
    return NORM_TABLE();
}

double Similarity::computeNorm(const String& fieldName, const FieldInvertStatePtr& state) {
    return (double)(state->getBoost() * lengthNorm(fieldName, state->getLength()));
}

uint8_t Similarity::encodeNorm(double f) {
    return SmallDouble::doubleToByte(f);
}

double Similarity::tf(int32_t freq) {
    return tf((double)freq);
}

IDFExplanationPtr Similarity::idfExplain(const TermPtr& term, const SearcherPtr& searcher) {
    int32_t df = searcher->docFreq(term);
    int32_t max = searcher->maxDoc();
    double _idf = idf(df, max);
    return newLucene<SimilarityIDFExplanation>(df, max, _idf);
}

IDFExplanationPtr Similarity::idfExplain(Collection<TermPtr> terms, const SearcherPtr& searcher) {
    int32_t max = searcher->maxDoc();
    double _idf = 0.0;
    String exp;
    for (Collection<TermPtr>::iterator term = terms.begin(); term != terms.end(); ++term) {
        int32_t df = searcher->docFreq(*term);
        _idf += idf(df, max);
        exp += L" " + (*term)->text() + L"=" + StringUtils::toString(df);
    }
    return newLucene<SimilarityIDFExplanation>(exp, _idf);
}

double Similarity::scorePayload(int32_t docId, const String& fieldName, int32_t start, int32_t end, ByteArray payload, int32_t offset, int32_t length) {
    return 1.0;
}

SimilarityIDFExplanation::SimilarityIDFExplanation(int32_t df, int32_t max, double idf) {
    this->df = df;
    this->max = max;
    this->idf = idf;
}

SimilarityIDFExplanation::SimilarityIDFExplanation(const String& exp, double idf) {
    this->exp = exp;
    this->idf = idf;
}

SimilarityIDFExplanation::~SimilarityIDFExplanation() {
}

String SimilarityIDFExplanation::explain() {
    return !exp.empty() ? exp : L"idf(docFreq=" + StringUtils::toString(df) + L", maxDocs=" + StringUtils::toString(max) + L")";
}

double SimilarityIDFExplanation::getIdf() {
    return idf;
}

}
