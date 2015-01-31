/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/BooleanQuery.h"
#include "fnord-fts/search/_BooleanQuery.h"
#include "fnord-fts/search/BooleanScorer.h"
#include "fnord-fts/search/BooleanScorer2.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace fnord {
namespace fts {

int32_t BooleanQuery::maxClauseCount = 1024;

BooleanQuery::BooleanQuery(bool disableCoord) {
    this->disableCoord = disableCoord;
    this->clauses = Collection<BooleanClausePtr>::newInstance();
    this->minNrShouldMatch = 0;
}

BooleanQuery::~BooleanQuery() {
}

int32_t BooleanQuery::getMaxClauseCount() {
    return maxClauseCount;
}

void BooleanQuery::setMaxClauseCount(int32_t maxClauseCount) {
    if (maxClauseCount < 1) {
        boost::throw_exception(IllegalArgumentException(L"maxClauseCount must be >= 1"));
    }
    BooleanQuery::maxClauseCount = maxClauseCount;
}

bool BooleanQuery::isCoordDisabled() {
    return disableCoord;
}

SimilarityPtr BooleanQuery::getSimilarity(const SearcherPtr& searcher) {
    SimilarityPtr result(Query::getSimilarity(searcher));
    if (disableCoord) { // disable coord as requested
        result = newLucene<SimilarityDisableCoord>(result);
    }
    return result;
}

void BooleanQuery::setMinimumNumberShouldMatch(int32_t min) {
    this->minNrShouldMatch = min;
}

int32_t BooleanQuery::getMinimumNumberShouldMatch() {
    return minNrShouldMatch;
}

void BooleanQuery::add(const QueryPtr& query, BooleanClause::Occur occur) {
    add(newLucene<BooleanClause>(query, occur));
}

void BooleanQuery::add(const BooleanClausePtr& clause) {
    if (clauses.size() >= maxClauseCount) {
        boost::throw_exception(TooManyClausesException(L"maxClauseCount is set to " + StringUtils::toString(maxClauseCount)));
    }
    clauses.add(clause);
}

Collection<BooleanClausePtr> BooleanQuery::getClauses() {
    return clauses;
}

Collection<BooleanClausePtr>::iterator BooleanQuery::begin() {
    return clauses.begin();
}

Collection<BooleanClausePtr>::iterator BooleanQuery::end() {
    return clauses.end();
}

WeightPtr BooleanQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<BooleanWeight>(shared_from_this(), searcher);
}

QueryPtr BooleanQuery::rewrite(const IndexReaderPtr& reader) {
    if (minNrShouldMatch == 0 && clauses.size() == 1) { // optimize 1-clause queries
        BooleanClausePtr c(clauses[0]);
        if (!c->isProhibited()) { // just return clause
            QueryPtr query(c->getQuery()->rewrite(reader)); // rewrite first

            if (getBoost() != 1.0) { // incorporate boost
                if (query == c->getQuery()) { // if rewrite was no-op
                    query = std::dynamic_pointer_cast<Query>(query->clone());    // then clone before boost
                }
                query->setBoost(getBoost() * query->getBoost());
            }

            return query;
        }
    }

    BooleanQueryPtr clone; // recursively rewrite
    for (int32_t i = 0; i < clauses.size(); ++i) {
        BooleanClausePtr c(clauses[i]);
        QueryPtr query(c->getQuery()->rewrite(reader));
        if (query != c->getQuery()) { // clause rewrote: must clone
            if (!clone) {
                clone = std::dynamic_pointer_cast<BooleanQuery>(this->clone());
            }
            clone->clauses[i] = newLucene<BooleanClause>(query, c->getOccur());
        }
    }

    if (clone) {
        return clone;    // some clauses rewrote
    } else {
        return shared_from_this();    // no clauses rewrote
    }
}

void BooleanQuery::extractTerms(SetTerm terms) {
    for (Collection<BooleanClausePtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        (*clause)->getQuery()->extractTerms(terms);
    }
}

LuceneObjectPtr BooleanQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Query::clone(other ? other : newLucene<BooleanQuery>());
    BooleanQueryPtr cloneQuery(std::dynamic_pointer_cast<BooleanQuery>(clone));
    cloneQuery->disableCoord = disableCoord;
    cloneQuery->minNrShouldMatch = minNrShouldMatch;
    cloneQuery->clauses = Collection<BooleanClausePtr>::newInstance(clauses.begin(), clauses.end());
    return cloneQuery;
}

String BooleanQuery::toString(const String& field) {
    String buffer;
    bool needParens = (getBoost() != 1.0 || getMinimumNumberShouldMatch() > 0);
    if (needParens) {
        buffer += L"(";
    }

    for (Collection<BooleanClausePtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        if (clause != clauses.begin()) {
            buffer += L" ";
        }

        if ((*clause)->isProhibited()) {
            buffer += L"-";
        } else if ((*clause)->isRequired()) {
            buffer += L"+";
        }

        QueryPtr subQuery((*clause)->getQuery());
        if (subQuery) {
            if (std::dynamic_pointer_cast<BooleanQuery>(subQuery)) { // wrap sub-bools in parens
                buffer += L"(";
                buffer += subQuery->toString(field);
                buffer += L")";
            } else {
                buffer += subQuery->toString(field);
            }
        } else {
            buffer += L"null";
        }
    }

    if (needParens) {
        buffer += L")";
    }

    if (getMinimumNumberShouldMatch() > 0) {
        buffer += L"~";
        buffer += StringUtils::toString(getMinimumNumberShouldMatch());
    }

    if (getBoost() != 1.0) {
        buffer += boostString();
    }

    return buffer;
}

bool BooleanQuery::equals(const LuceneObjectPtr& other) {
    BooleanQueryPtr otherQuery(std::dynamic_pointer_cast<BooleanQuery>(other));
    if (!otherQuery) {
        return false;
    }
    return (getBoost() == otherQuery->getBoost() &&
            clauses.equals(otherQuery->clauses, luceneEquals<BooleanClausePtr>()) &&
            getMinimumNumberShouldMatch() == otherQuery->getMinimumNumberShouldMatch() &&
            disableCoord == otherQuery->disableCoord);
}

int32_t BooleanQuery::hashCode() {
    return MiscUtils::doubleToIntBits(getBoost()) ^ MiscUtils::hashCode(clauses.begin(), clauses.end(), MiscUtils::hashLucene<BooleanClausePtr>) +
           getMinimumNumberShouldMatch() + (disableCoord ? 17 : 0);
}

BooleanWeight::BooleanWeight(const BooleanQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
    weights = Collection<WeightPtr>::newInstance();
    for (Collection<BooleanClausePtr>::iterator clause = query->clauses.begin(); clause != query->clauses.end(); ++clause) {
        weights.add((*clause)->getQuery()->createWeight(searcher));
    }
}

BooleanWeight::~BooleanWeight() {
}

QueryPtr BooleanWeight::getQuery() {
    return query;
}

double BooleanWeight::getValue() {
    return query->getBoost();
}

double BooleanWeight::sumOfSquaredWeights() {
    double sum = 0.0;
    for (int32_t i = 0; i < weights.size(); ++i) {
        // call sumOfSquaredWeights for all clauses in case of side effects
        double s = weights[i]->sumOfSquaredWeights(); // sum sub weights
        if (!query->clauses[i]->isProhibited()) {
            // only add to sum for non-prohibited clauses
            sum += s;
        }
    }

    sum *= query->getBoost() * query->getBoost(); // boost each sub-weight

    return sum;
}

void BooleanWeight::normalize(double norm) {
    norm *= query->getBoost(); // incorporate boost
    for (Collection<WeightPtr>::iterator w = weights.begin(); w != weights.end(); ++w) {
        // normalize all clauses, (even if prohibited in case of side affects)
        (*w)->normalize(norm);
    }
}

ExplanationPtr BooleanWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    int32_t minShouldMatch = query->getMinimumNumberShouldMatch();
    ComplexExplanationPtr sumExpl(newLucene<ComplexExplanation>());
    sumExpl->setDescription(L"sum of:");
    int32_t coord = 0;
    int32_t maxCoord = 0;
    double sum = 0.0;
    bool fail = false;
    int32_t shouldMatchCount = 0;
    Collection<BooleanClausePtr>::iterator c = query->clauses.begin();
    for (Collection<WeightPtr>::iterator w = weights.begin(); w != weights.end(); ++w, ++c) {
        if (!(*w)->scorer(reader, true, true)) {
            continue;
        }
        ExplanationPtr e((*w)->explain(reader, doc));
        if (!(*c)->isProhibited()) {
            ++maxCoord;
        }
        if (e->isMatch()) {
            if (!(*c)->isProhibited()) {
                sumExpl->addDetail(e);
                sum += e->getValue();
                ++coord;
            } else {
                ExplanationPtr r(newLucene<Explanation>(0.0, L"match on prohibited clause (" + (*c)->getQuery()->toString() + L")"));
                r->addDetail(e);
                sumExpl->addDetail(r);
                fail = true;
            }
            if ((*c)->getOccur() == BooleanClause::SHOULD) {
                ++shouldMatchCount;
            }
        } else if ((*c)->isRequired()) {
            ExplanationPtr r(newLucene<Explanation>(0.0, L"no match on required clause (" + (*c)->getQuery()->toString() + L")"));
            r->addDetail(e);
            sumExpl->addDetail(r);
            fail = true;
        }
    }
    if (fail) {
        sumExpl->setMatch(false);
        sumExpl->setValue(0.0);
        sumExpl->setDescription(L"Failure to meet condition(s) of required/prohibited clause(s)");
        return sumExpl;
    } else if (shouldMatchCount < minShouldMatch) {
        sumExpl->setMatch(false);
        sumExpl->setValue(0.0);
        sumExpl->setDescription(L"Failure to match minimum number of optional clauses: " + StringUtils::toString(minShouldMatch));
        return sumExpl;
    }

    sumExpl->setMatch(0 < coord);
    sumExpl->setValue(sum);
    double coordFactor = similarity->coord(coord, maxCoord);
    if (coordFactor == 1.0) { // coord is no-op
        return sumExpl;    // eliminate wrapper
    } else {
        ComplexExplanationPtr result(newLucene<ComplexExplanation>(sumExpl->isMatch(), sum * coordFactor, L"product of:"));
        result->addDetail(sumExpl);
        result->addDetail(newLucene<Explanation>(coordFactor, L"coord(" + StringUtils::toString(coord) + L"/" + StringUtils::toString(maxCoord) + L")"));
        return result;
    }
}

ScorerPtr BooleanWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    Collection<ScorerPtr> required(Collection<ScorerPtr>::newInstance());
    Collection<ScorerPtr> prohibited(Collection<ScorerPtr>::newInstance());
    Collection<ScorerPtr> optional(Collection<ScorerPtr>::newInstance());
    Collection<BooleanClausePtr>::iterator c = query->clauses.begin();
    for (Collection<WeightPtr>::iterator w = weights.begin(); w != weights.end(); ++w, ++c) {
        ScorerPtr subScorer((*w)->scorer(reader, true, false));
        if (!subScorer) {
            if ((*c)->isRequired()) {
                return ScorerPtr();
            }
        } else if ((*c)->isRequired()) {
            required.add(subScorer);
        } else if ((*c)->isProhibited()) {
            prohibited.add(subScorer);
        } else {
            optional.add(subScorer);
        }
    }

    // Check if we can return a BooleanScorer
    if (!scoreDocsInOrder && topScorer && required.empty() && prohibited.size() < 32) {
        return newLucene<BooleanScorer>(similarity, query->minNrShouldMatch, optional, prohibited);
    }

    if (required.empty() && optional.empty()) {
        // no required and optional clauses.
        return ScorerPtr();
    } else if (optional.size() < query->minNrShouldMatch) {
        // either >1 req scorer, or there are 0 req scorers and at least 1 optional scorer. Therefore if there
        // are not enough optional scorers no documents will be matched by the query
        return ScorerPtr();
    }

    // Return a BooleanScorer2
    return newLucene<BooleanScorer2>(similarity, query->minNrShouldMatch, required, prohibited, optional);
}

bool BooleanWeight::scoresDocsOutOfOrder() {
    int32_t numProhibited = 0;
    for (Collection<BooleanClausePtr>::iterator c = query->clauses.begin(); c != query->clauses.end(); ++c) {
        if ((*c)->isRequired()) {
            return false;    // BS2 (in-order) will be used by scorer()
        } else if ((*c)->isProhibited()) {
            ++numProhibited;
        }
    }

    if (numProhibited > 32) { // cannot use BS
        return false;
    }

    // scorer() will return an out-of-order scorer if requested.
    return true;
}

SimilarityDisableCoord::SimilarityDisableCoord(const SimilarityPtr& delegee) : SimilarityDelegator(delegee) {
}

SimilarityDisableCoord::~SimilarityDisableCoord() {
}

double SimilarityDisableCoord::coord(int32_t overlap, int32_t maxOverlap) {
    return 1.0; // disable coord
}

}

}
