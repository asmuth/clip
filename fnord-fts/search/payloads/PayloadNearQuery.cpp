/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PayloadNearQuery.h"
#include "AveragePayloadFunction.h"
#include "IndexReader.h"
#include "NearSpansOrdered.h"
#include "NearSpansUnordered.h"
#include "Similarity.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

PayloadNearQuery::PayloadNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder) : SpanNearQuery(clauses, slop, inOrder) {
    fieldName = clauses[0]->getField(); // all clauses must have same field
    this->function = newLucene<AveragePayloadFunction>();
}

PayloadNearQuery::PayloadNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder, const PayloadFunctionPtr& function) : SpanNearQuery(clauses, slop, inOrder) {
    fieldName = clauses[0]->getField(); // all clauses must have same field
    this->function = function;
}

PayloadNearQuery::~PayloadNearQuery() {
}

WeightPtr PayloadNearQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<PayloadNearSpanWeight>(shared_from_this(), searcher);
}

LuceneObjectPtr PayloadNearQuery::clone(const LuceneObjectPtr& other) {
    int32_t sz = clauses.size();
    Collection<SpanQueryPtr> newClauses(Collection<SpanQueryPtr>::newInstance(sz));

    for (int32_t i = 0; i < sz; ++i) {
        newClauses[i] = boost::dynamic_pointer_cast<SpanQuery>(clauses[i]->clone());
    }

    PayloadNearQueryPtr payloadNearQuery(newLucene<PayloadNearQuery>(newClauses, slop, inOrder));
    payloadNearQuery->setBoost(getBoost());
    return payloadNearQuery;
}

String PayloadNearQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"payloadNear([";
    for (Collection<SpanQueryPtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        if (clause != clauses.begin()) {
            buffer << L", ";
        }
        buffer << (*clause)->toString(field);
    }
    buffer << L"], " << slop << L", " << inOrder << L")" << boostString();
    return buffer.str();
}

bool PayloadNearQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!SpanNearQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    PayloadNearQueryPtr otherQuery(boost::dynamic_pointer_cast<PayloadNearQuery>(other));
    if (!otherQuery) {
        return false;
    }
    if (fieldName != otherQuery->fieldName) {
        return false;
    }
    if (!function) {
        if (otherQuery->function) {
            return false;
        }
    } else if (!function->equals(otherQuery->function)) {
        return false;
    }
    return true;
}

int32_t PayloadNearQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = SpanNearQuery::hashCode();
    result = prime * result + (fieldName.empty() ? 0 : StringUtils::hashCode(fieldName));
    result = prime * result + (!function ? 0 : function->hashCode());
    return result;
}

PayloadNearSpanWeight::PayloadNearSpanWeight(const SpanQueryPtr& query, const SearcherPtr& searcher) : SpanWeight(query, searcher) {
}

PayloadNearSpanWeight::~PayloadNearSpanWeight() {
}

ScorerPtr PayloadNearSpanWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    return newLucene<PayloadNearSpanScorer>(query->getSpans(reader), shared_from_this(), similarity, reader->norms(query->getField()));
}

PayloadNearSpanScorer::PayloadNearSpanScorer(const SpansPtr& spans, const WeightPtr& weight, const SimilarityPtr& similarity, ByteArray norms) : SpanScorer(spans, weight, similarity, norms) {
    this->spans = spans;
    this->payloadScore = 0.0;
    this->payloadsSeen = 0;
    this->similarity = getSimilarity();
}

PayloadNearSpanScorer::~PayloadNearSpanScorer() {
}

void PayloadNearSpanScorer::getPayloads(Collection<SpansPtr> subSpans) {
    for (Collection<SpansPtr>::iterator span = subSpans.begin(); span != subSpans.end(); ++span) {
        if (MiscUtils::typeOf<NearSpansOrdered>(*span)) {
            NearSpansOrderedPtr ordered(boost::static_pointer_cast<NearSpansOrdered>(*span));
            if (ordered->isPayloadAvailable()) {
                processPayloads(ordered->getPayload(), ordered->start(), ordered->end());
            }
            getPayloads(ordered->getSubSpans());
        } else if (MiscUtils::typeOf<NearSpansUnordered>(*span)) {
            NearSpansUnorderedPtr unordered(boost::static_pointer_cast<NearSpansUnordered>(*span));
            if (unordered->isPayloadAvailable()) {
                processPayloads(unordered->getPayload(), unordered->start(), unordered->end());
            }
            getPayloads(unordered->getSubSpans());
        }
    }
}

void PayloadNearSpanScorer::processPayloads(Collection<ByteArray> payLoads, int32_t start, int32_t end) {
    PayloadNearSpanWeightPtr spanWeight(boost::static_pointer_cast<PayloadNearSpanWeight>(weight));
    PayloadNearQueryPtr nearQuery(boost::static_pointer_cast<PayloadNearQuery>(spanWeight->query));

    for (Collection<ByteArray>::iterator payload = payLoads.begin(); payload != payLoads.end(); ++payload) {
        payloadScore = nearQuery->function->currentScore(doc, nearQuery->fieldName, start, end, payloadsSeen, payloadScore,
                       similarity->scorePayload(doc, nearQuery->fieldName, spans->start(),
                                                spans->end(), *payload, 0, payload->size()));
        ++payloadsSeen;
    }
}

bool PayloadNearSpanScorer::setFreqCurrentDoc() {
    if (!more) {
        return false;
    }
    Collection<SpansPtr> spansArr(newCollection<SpansPtr>(spans));
    payloadScore = 0.0;
    payloadsSeen = 0;
    getPayloads(spansArr);
    return SpanScorer::setFreqCurrentDoc();
}

double PayloadNearSpanScorer::score() {
    PayloadNearSpanWeightPtr spanWeight(boost::static_pointer_cast<PayloadNearSpanWeight>(weight));
    PayloadNearQueryPtr nearQuery(boost::static_pointer_cast<PayloadNearQuery>(spanWeight->query));
    return SpanScorer::score() * nearQuery->function->docScore(doc, nearQuery->fieldName, payloadsSeen, payloadScore);
}

ExplanationPtr PayloadNearSpanScorer::explain(int32_t doc) {
    ExplanationPtr result(newLucene<Explanation>());
    ExplanationPtr nonPayloadExpl(SpanScorer::explain(doc));
    result->addDetail(nonPayloadExpl);
    ExplanationPtr payloadBoost(newLucene<Explanation>());
    result->addDetail(payloadBoost);
    double avgPayloadScore = (payloadsSeen > 0 ? (payloadScore / (double)payloadsSeen) : 1.0);
    payloadBoost->setValue(avgPayloadScore);
    payloadBoost->setDescription(L"scorePayload(...)");
    result->setValue(nonPayloadExpl->getValue() * avgPayloadScore);
    result->setDescription(L"bnq, product of:");
    return result;
}

}
