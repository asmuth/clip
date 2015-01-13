/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanScorer.h"
#include "Explanation.h"
#include "Weight.h"
#include "Similarity.h"
#include "Spans.h"
#include "StringUtils.h"

namespace Lucene {

SpanScorer::SpanScorer(const SpansPtr& spans, const WeightPtr& weight, const SimilarityPtr& similarity, ByteArray norms) : Scorer(similarity) {
    this->spans = spans;
    this->norms = norms;
    this->weight = weight;
    this->value = weight->getValue();
    this->freq = 0.0;
    if (this->spans->next()) {
        doc = -1;
        more = true;
    } else {
        doc = NO_MORE_DOCS;
        more = false;
    }
}

SpanScorer::~SpanScorer() {
}

int32_t SpanScorer::nextDoc() {
    if (!setFreqCurrentDoc()) {
        doc = NO_MORE_DOCS;
    }
    return doc;
}

int32_t SpanScorer::advance(int32_t target) {
    if (!more) {
        doc = NO_MORE_DOCS;
        return doc;
    }
    if (spans->doc() < target) { // setFreqCurrentDoc() leaves spans->doc() ahead
        more = spans->skipTo(target);
    }
    if (!setFreqCurrentDoc()) {
        doc = NO_MORE_DOCS;
    }
    return doc;
}

bool SpanScorer::setFreqCurrentDoc() {
    if (!more) {
        return false;
    }
    doc = spans->doc();
    freq = 0.0;
    do {
        int32_t matchLength = spans->end() - spans->start();
        freq += getSimilarity()->sloppyFreq(matchLength);
        more = spans->next();
    } while (more && (doc == spans->doc()));
    return true;
}

int32_t SpanScorer::docID() {
    return doc;
}

double SpanScorer::score() {
    double raw = getSimilarity()->tf(freq) * value; // raw score
    return norms ? raw * Similarity::decodeNorm(norms[doc]) : raw; // normalize
}

ExplanationPtr SpanScorer::explain(int32_t doc) {
    ExplanationPtr tfExplanation(newLucene<Explanation>());

    int32_t expDoc = advance(doc);

    double phraseFreq = expDoc == doc ? freq : 0.0;
    tfExplanation->setValue(getSimilarity()->tf(phraseFreq));
    tfExplanation->setDescription(L"tf(phraseFreq=" + StringUtils::toString(phraseFreq) + L")");

    return tfExplanation;
}

}
