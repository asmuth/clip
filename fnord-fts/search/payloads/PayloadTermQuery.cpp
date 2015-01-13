/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PayloadTermQuery.h"
#include "_PayloadTermQuery.h"
#include "Term.h"
#include "TermSpans.h"
#include "TermPositions.h"
#include "ComplexExplanation.h"
#include "IndexReader.h"
#include "Similarity.h"
#include "PayloadFunction.h"
#include "MiscUtils.h"

namespace Lucene {

PayloadTermQuery::PayloadTermQuery(const TermPtr& term, const PayloadFunctionPtr& function, bool includeSpanScore) : SpanTermQuery(term) {
    this->function = function;
    this->includeSpanScore = includeSpanScore;
}

PayloadTermQuery::~PayloadTermQuery() {
}

WeightPtr PayloadTermQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<PayloadTermWeight>(shared_from_this(), searcher);
}

LuceneObjectPtr PayloadTermQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = SpanQuery::clone(other ? other : newLucene<PayloadTermQuery>(term, function, includeSpanScore));
    PayloadTermQueryPtr termQuery(boost::dynamic_pointer_cast<PayloadTermQuery>(clone));
    termQuery->function = function;
    termQuery->includeSpanScore = includeSpanScore;
    return termQuery;
}

bool PayloadTermQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!SpanTermQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    PayloadTermQueryPtr otherQuery(boost::dynamic_pointer_cast<PayloadTermQuery>(other));
    if (!otherQuery) {
        return false;
    }
    if (!function) {
        if (otherQuery->function) {
            return false;
        }
    } else if (!function->equals(otherQuery->function)) {
        return false;
    }
    if (includeSpanScore != otherQuery->includeSpanScore) {
        return false;
    }
    return true;
}

int32_t PayloadTermQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = SpanTermQuery::hashCode();
    result = prime * result + (function ? function->hashCode() : 0);
    result = prime * result + (includeSpanScore ? 1231 : 1237);
    return result;
}

PayloadTermWeight::PayloadTermWeight(const PayloadTermQueryPtr& query, const SearcherPtr& searcher) : SpanWeight(query, searcher) {
}

PayloadTermWeight::~PayloadTermWeight() {
}

ScorerPtr PayloadTermWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    return newLucene<PayloadTermSpanScorer>(boost::dynamic_pointer_cast<TermSpans>(query->getSpans(reader)), shared_from_this(), similarity, reader->norms(query->getField()));
}

PayloadTermSpanScorer::PayloadTermSpanScorer(const TermSpansPtr& spans, const WeightPtr& weight, const SimilarityPtr& similarity, ByteArray norms) : SpanScorer(spans, weight, similarity, norms) {
    positions = spans->getPositions();
    payload = ByteArray::newInstance(256);
    payloadScore = 0.0;
    payloadsSeen = 0;
}

PayloadTermSpanScorer::~PayloadTermSpanScorer() {
}

bool PayloadTermSpanScorer::setFreqCurrentDoc() {
    if (!more) {
        return false;
    }
    doc = spans->doc();
    freq = 0.0;
    payloadScore = 0.0;
    payloadsSeen = 0;
    SimilarityPtr similarity1(getSimilarity());
    while (more && doc == spans->doc()) {
        int32_t matchLength = spans->end() - spans->start();

        freq += similarity1->sloppyFreq(matchLength);
        processPayload(similarity1);

        more = spans->next(); // this moves positions to the next match in this document
    }
    return more || (freq != 0);
}

void PayloadTermSpanScorer::processPayload(const SimilarityPtr& similarity) {
    if (positions->isPayloadAvailable()) {
        PayloadTermWeightPtr payloadWeight(boost::static_pointer_cast<PayloadTermWeight>(weight));
        PayloadTermQueryPtr payloadQuery(boost::static_pointer_cast<PayloadTermQuery>(payloadWeight->query));

        payload = positions->getPayload(payload, 0);
        payloadScore = payloadQuery->function->currentScore(doc, payloadQuery->term->field(), spans->start(), spans->end(),
                       payloadsSeen, payloadScore, similarity->scorePayload(doc,
                               payloadQuery->term->field(), spans->start(), spans->end(),
                               payload, 0, positions->getPayloadLength()));
        ++payloadsSeen;
    } else {
        // zero out the payload?
    }
}

double PayloadTermSpanScorer::score() {
    PayloadTermWeightPtr payloadWeight(boost::static_pointer_cast<PayloadTermWeight>(weight));
    PayloadTermQueryPtr payloadQuery(boost::static_pointer_cast<PayloadTermQuery>(payloadWeight->query));
    return payloadQuery->includeSpanScore ? getSpanScore() * getPayloadScore() : getPayloadScore();
}

double PayloadTermSpanScorer::getSpanScore() {
    return SpanScorer::score();
}

double PayloadTermSpanScorer::getPayloadScore() {
    PayloadTermWeightPtr payloadWeight(boost::static_pointer_cast<PayloadTermWeight>(weight));
    PayloadTermQueryPtr payloadQuery(boost::static_pointer_cast<PayloadTermQuery>(payloadWeight->query));
    return payloadQuery->function->docScore(doc, payloadQuery->term->field(), payloadsSeen, payloadScore);
}

ExplanationPtr PayloadTermSpanScorer::explain(int32_t doc) {
    ComplexExplanationPtr result(newLucene<ComplexExplanation>());
    ExplanationPtr nonPayloadExpl(SpanScorer::explain(doc));
    result->addDetail(nonPayloadExpl);

    ExplanationPtr payloadBoost(newLucene<Explanation>());
    result->addDetail(payloadBoost);

    double payloadScore = getPayloadScore();
    payloadBoost->setValue(payloadScore);
    payloadBoost->setDescription(L"scorePayload(...)");

    result->setValue(nonPayloadExpl->getValue() * payloadScore);
    result->setDescription(L"btq, product of:");
    result->setMatch(nonPayloadExpl->getValue() != 0.0);

    return result;
}

}
