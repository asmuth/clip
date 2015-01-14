/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/BooleanScorer2.h"
#include "fnord-fts/search/ReqOptSumScorer.h"
#include "fnord-fts/search/ReqExclScorer.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/search/Collector.h"

namespace fnord {
namespace fts {

BooleanScorer2::BooleanScorer2(const SimilarityPtr& similarity, int32_t minNrShouldMatch, Collection<ScorerPtr> required, Collection<ScorerPtr> prohibited, Collection<ScorerPtr> optional) : Scorer(similarity) {
    this->minNrShouldMatch = minNrShouldMatch;
    this->requiredScorers = required;
    this->prohibitedScorers = prohibited;
    this->optionalScorers = optional;
    this->doc = -1;
}

BooleanScorer2::~BooleanScorer2() {
}

void BooleanScorer2::initialize() {
    if (minNrShouldMatch < 0) {
        boost::throw_exception(IllegalArgumentException(L"Minimum number of optional scorers should not be negative"));
    }

    coordinator = newLucene<Coordinator>(shared_from_this());
    coordinator->maxCoord += optionalScorers.size();
    coordinator->maxCoord += requiredScorers.size();

    coordinator->init();
    countingSumScorer = makeCountingSumScorer();
}

ScorerPtr BooleanScorer2::countingDisjunctionSumScorer(Collection<ScorerPtr> scorers, int32_t minNrShouldMatch) {
    // each scorer from the list counted as a single matcher
    return newLucene<CountingDisjunctionSumScorer>(shared_from_this(), scorers, minNrShouldMatch);
}

ScorerPtr BooleanScorer2::countingConjunctionSumScorer(Collection<ScorerPtr> requiredScorers) {
    // each scorer from the list counted as a single matcher
    return newLucene<CountingConjunctionSumScorer>(shared_from_this(), Similarity::getDefault(), requiredScorers);
}

ScorerPtr BooleanScorer2::dualConjunctionSumScorer(const ScorerPtr& req1, const ScorerPtr& req2) {
    Collection<ScorerPtr> scorers(newCollection<ScorerPtr>(req1, req2));

    // All scorers match, so Similarity::getDefault() always has 1 as the coordination factor.
    // Therefore the sum of the scores of two scorers is used as score.
    return newLucene<ConjunctionScorer>(Similarity::getDefault(), scorers);
}

ScorerPtr BooleanScorer2::makeCountingSumScorer() {
    return requiredScorers.empty() ? makeCountingSumScorerNoReq() : makeCountingSumScorerSomeReq();
}

ScorerPtr BooleanScorer2::makeCountingSumScorerNoReq() {
    // minNrShouldMatch optional scorers are required, but at least 1
    int32_t nrOptRequired = minNrShouldMatch < 1 ? 1 : minNrShouldMatch;
    ScorerPtr requiredCountingSumScorer;
    if (optionalScorers.size() > nrOptRequired) {
        requiredCountingSumScorer = countingDisjunctionSumScorer(optionalScorers, nrOptRequired);
    } else if (optionalScorers.size() == 1) {
        requiredCountingSumScorer = newLucene<SingleMatchScorer>(optionalScorers[0], coordinator);
    } else {
        requiredCountingSumScorer = countingConjunctionSumScorer(optionalScorers);
    }
    return addProhibitedScorers(requiredCountingSumScorer);
}

ScorerPtr BooleanScorer2::makeCountingSumScorerSomeReq() {
    if (optionalScorers.size() == minNrShouldMatch) { // all optional scorers also required.
        Collection<ScorerPtr> allReq(Collection<ScorerPtr>::newInstance(requiredScorers.begin(), requiredScorers.end()));
        allReq.addAll(optionalScorers.begin(), optionalScorers.end());
        return addProhibitedScorers(countingConjunctionSumScorer(allReq));
    } else { // optionalScorers.size() > minNrShouldMatch, and at least one required scorer
        ScorerPtr requiredCountingSumScorer = requiredScorers.size() == 1 ? newLucene<SingleMatchScorer>(requiredScorers[0], coordinator) : countingConjunctionSumScorer(requiredScorers);
        if (minNrShouldMatch > 0) { // use a required disjunction scorer over the optional scorers
            return addProhibitedScorers(dualConjunctionSumScorer(requiredCountingSumScorer, countingDisjunctionSumScorer(optionalScorers, minNrShouldMatch)));
        } else { // minNrShouldMatch == 0
            return newLucene<ReqOptSumScorer>(addProhibitedScorers(requiredCountingSumScorer), optionalScorers.size() == 1 ? newLucene<SingleMatchScorer>(optionalScorers[0], coordinator) : countingDisjunctionSumScorer(optionalScorers, 1));
        }
    }
}

ScorerPtr BooleanScorer2::addProhibitedScorers(const ScorerPtr& requiredCountingSumScorer) {
    return prohibitedScorers.empty() ? requiredCountingSumScorer : newLucene<ReqExclScorer>(requiredCountingSumScorer, (prohibitedScorers.size() == 1 ? prohibitedScorers[0] : newLucene<DisjunctionSumScorer>(prohibitedScorers)));
}

void BooleanScorer2::score(const CollectorPtr& collector) {
    collector->setScorer(shared_from_this());
    while ((doc = countingSumScorer->nextDoc()) != NO_MORE_DOCS) {
        collector->collect(doc);
    }
}

bool BooleanScorer2::score(const CollectorPtr& collector, int32_t max, int32_t firstDocID) {
    doc = firstDocID;
    collector->setScorer(shared_from_this());
    while (doc < max) {
        collector->collect(doc);
        doc = countingSumScorer->nextDoc();
    }
    return (doc != NO_MORE_DOCS);
}

int32_t BooleanScorer2::docID() {
    return doc;
}

int32_t BooleanScorer2::nextDoc() {
    doc = countingSumScorer->nextDoc();
    return doc;
}

double BooleanScorer2::score() {
    coordinator->nrMatchers = 0;
    double sum = countingSumScorer->score();
    return sum * coordinator->coordFactors[coordinator->nrMatchers];
}

int32_t BooleanScorer2::advance(int32_t target) {
    doc = countingSumScorer->advance(target);
    return doc;
}

Coordinator::Coordinator(const BooleanScorer2Ptr& scorer) {
    _scorer = scorer;
    maxCoord = 0;
    nrMatchers = 0;
}

Coordinator::~Coordinator() {
}

void Coordinator::init() {
    coordFactors = Collection<double>::newInstance(maxCoord + 1);
    SimilarityPtr sim(BooleanScorer2Ptr(_scorer)->getSimilarity());
    for (int32_t i = 0; i <= maxCoord; ++i) {
        coordFactors[i] = sim->coord(i, maxCoord);
    }
}

SingleMatchScorer::SingleMatchScorer(const ScorerPtr& scorer, const CoordinatorPtr& coordinator) : Scorer(scorer->getSimilarity()) {
    lastScoredDoc = -1;
    lastDocScore = std::numeric_limits<double>::quiet_NaN();
    this->scorer = scorer;
    this->coordinator = coordinator;
}

SingleMatchScorer::~SingleMatchScorer() {
}

double SingleMatchScorer::score() {
    int32_t doc = docID();
    if (doc >= lastScoredDoc) {
        if (doc > lastScoredDoc) {
            lastDocScore = scorer->score();
            lastScoredDoc = doc;
        }
        ++coordinator->nrMatchers;
    }
    return lastDocScore;
}

int32_t SingleMatchScorer::docID() {
    return scorer->docID();
}

int32_t SingleMatchScorer::nextDoc() {
    return scorer->nextDoc();
}

int32_t SingleMatchScorer::advance(int32_t target) {
    return scorer->advance(target);
}

CountingDisjunctionSumScorer::CountingDisjunctionSumScorer(const BooleanScorer2Ptr& scorer, Collection<ScorerPtr> subScorers, int32_t minimumNrMatchers) : DisjunctionSumScorer(subScorers, minimumNrMatchers) {
    _scorer = scorer;
    lastScoredDoc = -1;
    lastDocScore = std::numeric_limits<double>::quiet_NaN();
}

CountingDisjunctionSumScorer::~CountingDisjunctionSumScorer() {
}

double CountingDisjunctionSumScorer::score() {
    int32_t doc = docID();
    if (doc >= lastScoredDoc) {
        if (doc > lastScoredDoc) {
            lastDocScore = DisjunctionSumScorer::score();
            lastScoredDoc = doc;
        }
        BooleanScorer2Ptr(_scorer)->coordinator->nrMatchers += DisjunctionSumScorer::_nrMatchers;
    }
    return lastDocScore;
}

CountingConjunctionSumScorer::CountingConjunctionSumScorer(const BooleanScorer2Ptr& scorer, const SimilarityPtr& similarity, Collection<ScorerPtr> scorers) : ConjunctionScorer(similarity, scorers) {
    _scorer = scorer;
    lastScoredDoc = -1;
    requiredNrMatchers = scorers.size();
    lastDocScore = std::numeric_limits<double>::quiet_NaN();
}

CountingConjunctionSumScorer::~CountingConjunctionSumScorer() {
}

double CountingConjunctionSumScorer::score() {
    int32_t doc = docID();
    if (doc >= lastScoredDoc) {
        if (doc > lastScoredDoc) {
            lastDocScore = ConjunctionScorer::score();
            lastScoredDoc = doc;
        }
        BooleanScorer2Ptr(_scorer)->coordinator->nrMatchers += requiredNrMatchers;
    }
    // All scorers match, so Similarity::getDefault() ConjunctionScorer::score() always has 1 as the
    /// coordination factor.  Therefore the sum of the scores of the requiredScorers is used as score.
    return lastDocScore;
}

}

}
