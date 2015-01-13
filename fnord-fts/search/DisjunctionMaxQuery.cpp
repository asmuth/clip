/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/DisjunctionMaxQuery.h"
#include "fnord-fts/search/_DisjunctionMaxQuery.h"
#include "fnord-fts/search/BooleanQuery.h"
#include "fnord-fts/search/DocIdSetIterator.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/search/Searcher.h"
#include "fnord-fts/search/DisjunctionMaxScorer.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

DisjunctionMaxQuery::DisjunctionMaxQuery(double tieBreakerMultiplier) {
    this->tieBreakerMultiplier = tieBreakerMultiplier;
    this->disjuncts = Collection<QueryPtr>::newInstance();
}

DisjunctionMaxQuery::DisjunctionMaxQuery(Collection<QueryPtr> disjuncts, double tieBreakerMultiplier) {
    this->tieBreakerMultiplier = tieBreakerMultiplier;
    this->disjuncts = Collection<QueryPtr>::newInstance();
    add(disjuncts);
}

DisjunctionMaxQuery::~DisjunctionMaxQuery() {
}

void DisjunctionMaxQuery::add(const QueryPtr& query) {
    disjuncts.add(query);
}

void DisjunctionMaxQuery::add(Collection<QueryPtr> disjuncts) {
    this->disjuncts.addAll(disjuncts.begin(), disjuncts.end());
}

Collection<QueryPtr>::iterator DisjunctionMaxQuery::begin() {
    return disjuncts.begin();
}

Collection<QueryPtr>::iterator DisjunctionMaxQuery::end() {
    return disjuncts.end();
}

WeightPtr DisjunctionMaxQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<DisjunctionMaxWeight>(shared_from_this(), searcher);
}

QueryPtr DisjunctionMaxQuery::rewrite(const IndexReaderPtr& reader) {
    int32_t numDisjunctions = disjuncts.size();
    if (numDisjunctions == 1) {
        QueryPtr singleton(disjuncts[0]);
        QueryPtr result(singleton->rewrite(reader));
        if (getBoost() != 1.0) {
            if (result == singleton) {
                result = std::dynamic_pointer_cast<Query>(result->clone());
            }
            result->setBoost(getBoost() * result->getBoost());
        }
        return result;
    }
    DisjunctionMaxQueryPtr clone;
    for (int32_t i = 0; i < numDisjunctions; ++i) {
        QueryPtr clause(disjuncts[i]);
        QueryPtr rewrite(clause->rewrite(reader));
        if (rewrite != clause) {
            if (!clone) {
                clone = std::dynamic_pointer_cast<DisjunctionMaxQuery>(this->clone());
            }
            clone->disjuncts[i] = rewrite;
        }
    }
    return clone ? clone : shared_from_this();
}

LuceneObjectPtr DisjunctionMaxQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Query::clone(other ? other : newLucene<DisjunctionMaxQuery>());
    DisjunctionMaxQueryPtr cloneQuery(std::dynamic_pointer_cast<DisjunctionMaxQuery>(clone));
    cloneQuery->tieBreakerMultiplier = tieBreakerMultiplier;
    cloneQuery->disjuncts = Collection<QueryPtr>::newInstance(disjuncts.begin(), disjuncts.end());
    return cloneQuery;
}

void DisjunctionMaxQuery::extractTerms(SetTerm terms) {
    for (Collection<QueryPtr>::iterator query = disjuncts.begin(); query != disjuncts.end(); ++query) {
        (*query)->extractTerms(terms);
    }
}

String DisjunctionMaxQuery::toString(const String& field) {
    String buffer(L"(");
    for (Collection<QueryPtr>::iterator query = disjuncts.begin(); query != disjuncts.end(); ++query) {
        if (query != disjuncts.begin()) {
            buffer += L" | ";
        }
        if (std::dynamic_pointer_cast<BooleanQuery>(*query)) { // wrap sub-bools in parens
            buffer += L"(" + (*query)->toString(field) + L")";
        } else {
            buffer += (*query)->toString(field);
        }
    }
    buffer += L")";
    if (tieBreakerMultiplier != 0.0) {
        buffer += L"~" + StringUtils::toString(tieBreakerMultiplier);
    }
    if (getBoost() != 1.0) {
        buffer += L"^" + StringUtils::toString(getBoost());
    }
    return buffer;
}

bool DisjunctionMaxQuery::equals(const LuceneObjectPtr& other) {
    if (!Query::equals(other)) {
        return false;
    }

    DisjunctionMaxQueryPtr otherDisjunctionMaxQuery(std::dynamic_pointer_cast<DisjunctionMaxQuery>(other));
    if (!otherDisjunctionMaxQuery) {
        return false;
    }

    return (tieBreakerMultiplier == otherDisjunctionMaxQuery->tieBreakerMultiplier && disjuncts.equals(otherDisjunctionMaxQuery->disjuncts, luceneEquals<QueryPtr>()));
}

int32_t DisjunctionMaxQuery::hashCode() {
    return MiscUtils::doubleToIntBits(getBoost()) + MiscUtils::doubleToIntBits(tieBreakerMultiplier) + MiscUtils::hashCode(disjuncts.begin(), disjuncts.end(), MiscUtils::hashLucene<QueryPtr>);
}

DisjunctionMaxWeight::DisjunctionMaxWeight(const DisjunctionMaxQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = searcher->getSimilarity();
    this->weights = Collection<WeightPtr>::newInstance();
    for (Collection<QueryPtr>::iterator disjunctQuery = query->disjuncts.begin(); disjunctQuery != query->disjuncts.end(); ++disjunctQuery) {
        this->weights.add((*disjunctQuery)->createWeight(searcher));
    }
}

DisjunctionMaxWeight::~DisjunctionMaxWeight() {
}

QueryPtr DisjunctionMaxWeight::getQuery() {
    return query;
}

double DisjunctionMaxWeight::getValue() {
    return query->getBoost();
}

double DisjunctionMaxWeight::sumOfSquaredWeights() {
    double max = 0.0;
    double sum = 0.0;
    for (Collection<WeightPtr>::iterator currentWeight = weights.begin(); currentWeight != weights.end(); ++currentWeight) {
        double sub = (*currentWeight)->sumOfSquaredWeights();
        sum += sub;
        max = std::max(max, sub);
    }
    double boost = query->getBoost();
    return (((sum - max) * query->tieBreakerMultiplier * query->tieBreakerMultiplier) + max) * boost * boost;
}

void DisjunctionMaxWeight::normalize(double norm) {
    norm *= query->getBoost(); // Incorporate our boost
    for (Collection<WeightPtr>::iterator wt = weights.begin(); wt != weights.end(); ++wt) {
        (*wt)->normalize(norm);
    }
}

ScorerPtr DisjunctionMaxWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    Collection<ScorerPtr> scorers(Collection<ScorerPtr>::newInstance(weights.size()));
    int32_t idx = 0;
    for (Collection<WeightPtr>::iterator wt = weights.begin(); wt != weights.end(); ++wt) {
        ScorerPtr subScorer((*wt)->scorer(reader, true, false));
        if (subScorer && subScorer->nextDoc() != DocIdSetIterator::NO_MORE_DOCS) {
            scorers[idx++] = subScorer;
        }
    }
    if (idx == 0) {
        return ScorerPtr();    // all scorers did not have documents
    }
    DisjunctionMaxScorerPtr result(newLucene<DisjunctionMaxScorer>(query->tieBreakerMultiplier, similarity, scorers, idx));
    return result;
}

ExplanationPtr DisjunctionMaxWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    if (query->disjuncts.size() == 1) {
        return weights[0]->explain(reader, doc);
    }
    ComplexExplanationPtr result(newLucene<ComplexExplanation>());
    double max = 0.0;
    double sum = 0.0;
    result->setDescription(query->tieBreakerMultiplier == 0.0 ? L"max of:" : (L"max plus " + StringUtils::toString(query->tieBreakerMultiplier) + L" times others of:"));
    for (Collection<WeightPtr>::iterator wt = weights.begin(); wt != weights.end(); ++wt) {
        ExplanationPtr e = (*wt)->explain(reader, doc);
        if (e->isMatch()) {
            result->setMatch(true);
            result->addDetail(e);
            sum += e->getValue();
            max = std::max(max, e->getValue());
        }
    }
    result->setValue(max + (sum - max) * query->tieBreakerMultiplier);
    return result;
}

}
