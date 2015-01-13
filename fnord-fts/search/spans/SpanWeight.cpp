/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanWeight.h"
#include "SpanScorer.h"
#include "SpanQuery.h"
#include "IndexReader.h"
#include "ComplexExplanation.h"
#include "Similarity.h"
#include "StringUtils.h"

namespace Lucene {

SpanWeight::SpanWeight(const SpanQueryPtr& query, const SearcherPtr& searcher) {
    this->similarity = query->getSimilarity(searcher);
    this->query = query;

    terms = SetTerm::newInstance();
    query->extractTerms(terms);

    idfExp = similarity->idfExplain(Collection<TermPtr>::newInstance(terms.begin(), terms.end()), searcher);
    idf = idfExp->getIdf();
    value = 0.0;
    queryNorm = 0.0;
    queryWeight = 0.0;
}

SpanWeight::~SpanWeight() {
}

QueryPtr SpanWeight::getQuery() {
    return query;
}

double SpanWeight::getValue() {
    return value;
}

double SpanWeight::sumOfSquaredWeights() {
    queryWeight = idf * getQuery()->getBoost(); // compute query weight
    return queryWeight * queryWeight; // square it
}

void SpanWeight::normalize(double norm) {
    queryNorm = norm;
    queryWeight *= queryNorm; // normalize query weight
    value = queryWeight * idf; // idf for document
}

ScorerPtr SpanWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    return newLucene<SpanScorer>(query->getSpans(reader), shared_from_this(), similarity, reader->norms(query->getField()));
}

ExplanationPtr SpanWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ComplexExplanationPtr result(newLucene<ComplexExplanation>());
    result->setDescription(L"weight(" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");
    String field(query->getField());

    ExplanationPtr idfExpl(newLucene<Explanation>(idf, L"idf(" + field + L":" + idfExp->explain() + L")"));

    // explain query weight
    ExplanationPtr queryExpl(newLucene<Explanation>());
    queryExpl->setDescription(L"queryWeight(" + query->toString() + L"), product of:");

    ExplanationPtr boostExpl(newLucene<Explanation>(query->getBoost(), L"boost"));
    if (query->getBoost() != 1.0) {
        queryExpl->addDetail(boostExpl);
    }
    queryExpl->addDetail(idfExpl);

    ExplanationPtr queryNormExpl(newLucene<Explanation>(queryNorm, L"queryNorm"));
    queryExpl->addDetail(queryNormExpl);

    queryExpl->setValue(boostExpl->getValue() * idfExpl->getValue() * queryNormExpl->getValue());
    result->addDetail(queryExpl);

    // explain field weight
    ComplexExplanationPtr fieldExpl(newLucene<ComplexExplanation>());
    fieldExpl->setDescription(L"fieldWeight(" +    field + L":" + query->toString(field) + L" in " + StringUtils::toString(doc) + L"), product of:");

    ExplanationPtr tfExpl(boost::dynamic_pointer_cast<SpanScorer>(scorer(reader, true, false))->explain(doc));
    fieldExpl->addDetail(tfExpl);
    fieldExpl->addDetail(idfExpl);

    ExplanationPtr fieldNormExpl(newLucene<Explanation>());
    ByteArray fieldNorms(reader->norms(field));
    double fieldNorm = fieldNorms ? Similarity::decodeNorm(fieldNorms[doc]) : 1.0;
    fieldNormExpl->setValue(fieldNorm);
    fieldNormExpl->setDescription(L"fieldNorm(field=" + field + L", doc=" + StringUtils::toString(doc) + L")");
    fieldExpl->addDetail(fieldNormExpl);

    fieldExpl->setMatch(tfExpl->isMatch());
    fieldExpl->setValue(tfExpl->getValue() * idfExpl->getValue() * fieldNormExpl->getValue());

    result->addDetail(fieldExpl);
    result->setMatch(fieldExpl->getMatch());

    // combine them
    result->setValue(queryExpl->getValue() * fieldExpl->getValue());

    if (queryExpl->getValue() == 1.0) {
        return fieldExpl;
    }

    return result;
}

}
