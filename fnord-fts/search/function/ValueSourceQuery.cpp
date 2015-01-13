/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ValueSourceQuery.h"
#include "_ValueSourceQuery.h"
#include "ValueSource.h"
#include "DocValues.h"
#include "ComplexExplanation.h"
#include "IndexReader.h"
#include "TermDocs.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

ValueSourceQuery::ValueSourceQuery(const ValueSourcePtr& valSrc) {
    this->valSrc = valSrc;
}

ValueSourceQuery::~ValueSourceQuery() {
}

QueryPtr ValueSourceQuery::rewrite(const IndexReaderPtr& reader) {
    return shared_from_this();
}

void ValueSourceQuery::extractTerms(SetTerm terms) {
    // no terms involved here
}

WeightPtr ValueSourceQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<ValueSourceWeight>(shared_from_this(), searcher);
}

String ValueSourceQuery::toString(const String& field) {
    return valSrc->toString() + boostString();
}

bool ValueSourceQuery::equals(const LuceneObjectPtr& other) {
    ValueSourceQueryPtr otherQuery(boost::dynamic_pointer_cast<ValueSourceQuery>(other));
    if (!otherQuery) {
        return false;
    }
    return (getBoost() == otherQuery->getBoost() && valSrc->equals(otherQuery->valSrc));
}

int32_t ValueSourceQuery::hashCode() {
    return (StringUtils::hashCode(ValueSourceQuery::_getClassName()) + valSrc->hashCode()) ^ MiscUtils::doubleToIntBits(getBoost());
}

LuceneObjectPtr ValueSourceQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<ValueSourceQuery>(valSrc);
    ValueSourceQueryPtr cloneQuery(boost::dynamic_pointer_cast<ValueSourceQuery>(Query::clone(clone)));
    cloneQuery->valSrc = valSrc;
    return cloneQuery;
}

ValueSourceWeight::ValueSourceWeight(const ValueSourceQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
}

ValueSourceWeight::~ValueSourceWeight() {
}

QueryPtr ValueSourceWeight::getQuery() {
    return query;
}

double ValueSourceWeight::getValue() {
    return queryWeight;
}

double ValueSourceWeight::sumOfSquaredWeights() {
    queryWeight = query->getBoost();
    return queryWeight * queryWeight;
}

void ValueSourceWeight::normalize(double norm) {
    queryNorm = norm;
    queryWeight *= queryNorm;
}

ScorerPtr ValueSourceWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    return newLucene<ValueSourceScorer>(similarity, reader, shared_from_this());
}

ExplanationPtr ValueSourceWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    DocValuesPtr vals(query->valSrc->getValues(reader));
    double sc = queryWeight * vals->doubleVal(doc);

    ExplanationPtr result(newLucene<ComplexExplanation>(true, sc, query->toString() + L", product of:"));

    result->addDetail(vals->explain(doc));
    result->addDetail(newLucene<Explanation>(query->getBoost(), L"boost"));
    result->addDetail(newLucene<Explanation>(queryNorm, L"queryNorm"));
    return result;
}

ValueSourceScorer::ValueSourceScorer(const SimilarityPtr& similarity, const IndexReaderPtr& reader, const ValueSourceWeightPtr& weight) : Scorer(similarity) {
    this->weight = weight;
    this->qWeight = weight->getValue();
    this->doc = -1;
    // this is when/where the values are first created.
    vals = weight->query->valSrc->getValues(reader);
    termDocs = reader->termDocs(TermPtr());
}

ValueSourceScorer::~ValueSourceScorer() {
}

int32_t ValueSourceScorer::nextDoc() {
    doc = termDocs->next() ? termDocs->doc() : NO_MORE_DOCS;
    return doc;
}

int32_t ValueSourceScorer::docID() {
    return doc;
}

int32_t ValueSourceScorer::advance(int32_t target) {
    doc = termDocs->skipTo(target) ? termDocs->doc() : NO_MORE_DOCS;
    return doc;
}

double ValueSourceScorer::score() {
    return qWeight * vals->doubleVal(termDocs->doc());
}

}
