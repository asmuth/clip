/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/ConstantScoreQuery.h"
#include "fnord-fts/search/_ConstantScoreQuery.h"
#include "fnord-fts/search/Filter.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/search/DocIdSet.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace fnord {
namespace fts {

ConstantScoreQuery::ConstantScoreQuery(const FilterPtr& filter) {
    this->filter = filter;
}

ConstantScoreQuery::~ConstantScoreQuery() {
}

FilterPtr ConstantScoreQuery::getFilter() {
    return filter;
}

QueryPtr ConstantScoreQuery::rewrite(const IndexReaderPtr& reader) {
    return shared_from_this();
}

void ConstantScoreQuery::extractTerms(SetTerm terms) {
    // OK to not add any terms when used for MultiSearcher, but may not be OK for highlighting
}

WeightPtr ConstantScoreQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<ConstantWeight>(shared_from_this(), searcher);
}

String ConstantScoreQuery::toString(const String& field) {
    return L"ConstantScore(" + filter->toString() + (getBoost() == 1.0 ? L")" : L"^" + StringUtils::toString(getBoost()));
}

bool ConstantScoreQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    ConstantScoreQueryPtr otherConstantScoreQuery(std::dynamic_pointer_cast<ConstantScoreQuery>(other));
    if (!otherConstantScoreQuery) {
        return false;
    }

    return (this->getBoost() == otherConstantScoreQuery->getBoost() && this->filter->equals(otherConstantScoreQuery->filter));
}

int32_t ConstantScoreQuery::hashCode() {
    // Simple add is OK since no existing filter hashcode has a float component.
    return filter->hashCode() + MiscUtils::doubleToIntBits(getBoost());
}

LuceneObjectPtr ConstantScoreQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<ConstantScoreQuery>(filter);
    ConstantScoreQueryPtr cloneQuery(std::dynamic_pointer_cast<ConstantScoreQuery>(Query::clone(clone)));
    cloneQuery->filter = filter;
    return cloneQuery;
}

ConstantWeight::ConstantWeight(const ConstantScoreQueryPtr& constantScorer, const SearcherPtr& searcher) {
    this->constantScorer = constantScorer;
    this->similarity = constantScorer->getSimilarity(searcher);
    this->queryNorm = 0;
    this->queryWeight = 0;
}

ConstantWeight::~ConstantWeight() {
}

QueryPtr ConstantWeight::getQuery() {
    return constantScorer;
}

double ConstantWeight::getValue() {
    return queryWeight;
}

double ConstantWeight::sumOfSquaredWeights() {
    queryWeight = constantScorer->getBoost();
    return queryWeight * queryWeight;
}

void ConstantWeight::normalize(double norm) {
    this->queryNorm = norm;
    queryWeight *= this->queryNorm;
}

ScorerPtr ConstantWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    return newLucene<ConstantScorer>(constantScorer, similarity, reader, shared_from_this());
}

ExplanationPtr ConstantWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ConstantScorerPtr cs(newLucene<ConstantScorer>(constantScorer, similarity, reader, shared_from_this()));
    bool exists = (cs->docIdSetIterator->advance(doc) == doc);

    ComplexExplanationPtr result(newLucene<ComplexExplanation>());

    if (exists) {
        result->setDescription(L"ConstantScoreQuery(" + constantScorer->filter->toString() + L"), product of:");
        result->setValue(queryWeight);
        result->setMatch(true);
        result->addDetail(newLucene<Explanation>(constantScorer->getBoost(), L"boost"));
        result->addDetail(newLucene<Explanation>(queryNorm, L"queryNorm"));
    } else {
        result->setDescription(L"ConstantScoreQuery(" + constantScorer->filter->toString() + L") doesn't match id " + StringUtils::toString(doc));
        result->setValue(0);
        result->setMatch(false);
    }
    return result;
}

ConstantScorer::ConstantScorer(const ConstantScoreQueryPtr& constantScorer, const SimilarityPtr& similarity, const IndexReaderPtr& reader, const WeightPtr& w) : Scorer(similarity) {
    doc = -1;
    theScore = w->getValue();
    DocIdSetPtr docIdSet(constantScorer->filter->getDocIdSet(reader));
    if (!docIdSet) {
        docIdSetIterator = DocIdSet::EMPTY_DOCIDSET()->iterator();
    } else {
        DocIdSetIteratorPtr iter(docIdSet->iterator());
        if (!iter) {
            docIdSetIterator = DocIdSet::EMPTY_DOCIDSET()->iterator();
        } else {
            docIdSetIterator = iter;
        }
    }
}

ConstantScorer::~ConstantScorer() {
}

int32_t ConstantScorer::nextDoc() {
    return docIdSetIterator->nextDoc();
}

int32_t ConstantScorer::docID() {
    return docIdSetIterator->docID();
}

double ConstantScorer::score() {
    return theScore;
}

int32_t ConstantScorer::advance(int32_t target) {
    return docIdSetIterator->advance(target);
}

}

}
