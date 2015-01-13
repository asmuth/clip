/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/FilteredQuery.h"
#include "fnord-fts/_FilteredQuery.h"
#include "fnord-fts/Explanation.h"
#include "fnord-fts/Filter.h"
#include "fnord-fts/DocIdSet.h"
#include "fnord-fts/MiscUtils.h"

namespace Lucene {

FilteredQuery::FilteredQuery(const QueryPtr& query, const FilterPtr& filter) {
    this->query = query;
    this->filter = filter;
}

FilteredQuery::~FilteredQuery() {
}

WeightPtr FilteredQuery::createWeight(const SearcherPtr& searcher) {
    WeightPtr weight(query->createWeight(searcher));
    SimilarityPtr similarity(query->getSimilarity(searcher));
    return newLucene<FilteredQueryWeight>(shared_from_this(), weight, similarity);
}

QueryPtr FilteredQuery::rewrite(const IndexReaderPtr& reader) {
    QueryPtr rewritten(query->rewrite(reader));
    if (rewritten != query) {
        FilteredQueryPtr cloneQuery(boost::dynamic_pointer_cast<FilteredQuery>(clone()));
        cloneQuery->query = rewritten;
        return cloneQuery;
    } else {
        return shared_from_this();
    }
}

QueryPtr FilteredQuery::getQuery() {
    return query;
}

FilterPtr FilteredQuery::getFilter() {
    return filter;
}

void FilteredQuery::extractTerms(SetTerm terms) {
    getQuery()->extractTerms(terms);
}

String FilteredQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"filtered(" << query->toString(field) << L")->" << filter->toString() << boostString();
    return buffer.str();
}

bool FilteredQuery::equals(const LuceneObjectPtr& other) {
    FilteredQueryPtr otherFilteredQuery(boost::dynamic_pointer_cast<FilteredQuery>(other));
    if (!otherFilteredQuery) {
        return false;
    }
    return (Query::equals(other) && query->equals(otherFilteredQuery->query) && filter->equals(otherFilteredQuery->filter));
}

int32_t FilteredQuery::hashCode() {
    return query->hashCode() ^ filter->hashCode() + MiscUtils::doubleToIntBits(getBoost());
}

LuceneObjectPtr FilteredQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<FilteredQuery>(query, filter);
    FilteredQueryPtr cloneQuery(boost::dynamic_pointer_cast<FilteredQuery>(Query::clone(clone)));
    cloneQuery->query = query;
    cloneQuery->filter = filter;
    return cloneQuery;
}

FilteredQueryWeight::FilteredQueryWeight(const FilteredQueryPtr& query, const WeightPtr& weight, const SimilarityPtr& similarity) {
    this->query = query;
    this->weight = weight;
    this->similarity = similarity;
    value = 0.0;
}

FilteredQueryWeight::~FilteredQueryWeight() {
}

double FilteredQueryWeight::getValue() {
    return value;
}

double FilteredQueryWeight::sumOfSquaredWeights() {
    return weight->sumOfSquaredWeights() * query->getBoost() * query->getBoost();
}

void FilteredQueryWeight::normalize(double norm) {
    weight->normalize(norm);
    value = weight->getValue() * query->getBoost();
}

ExplanationPtr FilteredQueryWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ExplanationPtr inner(weight->explain(reader, doc));
    if (query->getBoost() !=1) {
        ExplanationPtr preBoost(inner);
        inner = newLucene<Explanation>(inner->getValue() * query->getBoost(), L"product of:");
        inner->addDetail(newLucene<Explanation>(query->getBoost(), L"boost"));
        inner->addDetail(preBoost);
    }
    FilterPtr f(query->filter);
    DocIdSetPtr docIdSet(f->getDocIdSet(reader));
    DocIdSetIteratorPtr docIdSetIterator(!docIdSet ? DocIdSet::EMPTY_DOCIDSET()->iterator() : docIdSet->iterator());
    if (!docIdSetIterator) {
        docIdSetIterator = DocIdSet::EMPTY_DOCIDSET()->iterator();
    }
    if (docIdSetIterator->advance(doc) == doc) {
        return inner;
    } else {
        ExplanationPtr result(newLucene<Explanation>(0.0, L"failure to match filter: " + f->toString()));
        result->addDetail(inner);
        return result;
    }
}

QueryPtr FilteredQueryWeight::getQuery() {
    return query;
}

ScorerPtr FilteredQueryWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    ScorerPtr scorer(weight->scorer(reader, true, false));
    if (!scorer) {
        return ScorerPtr();
    }
    DocIdSetPtr docIdSet(query->filter->getDocIdSet(reader));
    if (!docIdSet) {
        return ScorerPtr();
    }
    DocIdSetIteratorPtr docIdSetIterator(docIdSet->iterator());
    if (!docIdSetIterator) {
        return ScorerPtr();
    }
    return newLucene<FilteredQueryWeightScorer>(shared_from_this(), scorer, docIdSetIterator, similarity);
}

FilteredQueryWeightScorer::FilteredQueryWeightScorer(const FilteredQueryWeightPtr& weight, const ScorerPtr& scorer, const DocIdSetIteratorPtr& docIdSetIterator, const SimilarityPtr& similarity) : Scorer(similarity) {
    this->weight = weight;
    this->scorer = scorer;
    this->docIdSetIterator = docIdSetIterator;
    doc = -1;
}

FilteredQueryWeightScorer::~FilteredQueryWeightScorer() {
}

int32_t FilteredQueryWeightScorer::advanceToCommon(int32_t scorerDoc, int32_t disiDoc) {
    while (scorerDoc != disiDoc) {
        if (scorerDoc < disiDoc) {
            scorerDoc = scorer->advance(disiDoc);
        } else {
            disiDoc = docIdSetIterator->advance(scorerDoc);
        }
    }
    return scorerDoc;
}

int32_t FilteredQueryWeightScorer::nextDoc() {
    int32_t disiDoc = docIdSetIterator->nextDoc();
    int32_t scorerDoc = scorer->nextDoc();
    doc = (scorerDoc != NO_MORE_DOCS && advanceToCommon(scorerDoc, disiDoc) != NO_MORE_DOCS) ? scorer->docID() : NO_MORE_DOCS;
    return doc;
}

int32_t FilteredQueryWeightScorer::docID() {
    return doc;
}

int32_t FilteredQueryWeightScorer::advance(int32_t target) {
    int32_t disiDoc = docIdSetIterator->advance(target);
    int32_t scorerDoc = scorer->advance(target);
    doc = (scorerDoc != NO_MORE_DOCS && advanceToCommon(scorerDoc, disiDoc) != NO_MORE_DOCS) ? scorer->docID() : NO_MORE_DOCS;
    return doc;
}

double FilteredQueryWeightScorer::score() {
    return weight->query->getBoost() * scorer->score();
}

}
