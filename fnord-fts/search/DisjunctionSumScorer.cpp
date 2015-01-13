/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/DisjunctionSumScorer.h"
#include "fnord-fts/ScorerDocQueue.h"
#include "fnord-fts/Collector.h"

namespace Lucene {

DisjunctionSumScorer::DisjunctionSumScorer(Collection<ScorerPtr> subScorers, int32_t minimumNrMatchers) : Scorer(SimilarityPtr()) {
    this->currentDoc = -1;
    this->_nrMatchers = -1;
    this->currentScore = std::numeric_limits<double>::quiet_NaN();

    this->nrScorers = subScorers.size();

    if (minimumNrMatchers <= 0) {
        boost::throw_exception(IllegalArgumentException(L"Minimum nr of matchers must be positive"));
    }
    if (nrScorers <= 1) {
        boost::throw_exception(IllegalArgumentException(L"There must be at least 2 subScorers"));
    }

    this->minimumNrMatchers = minimumNrMatchers;
    this->subScorers = subScorers;
}

DisjunctionSumScorer::~DisjunctionSumScorer() {
}

void DisjunctionSumScorer::initialize() {
    initScorerDocQueue();
}

void DisjunctionSumScorer::initScorerDocQueue() {
    scorerDocQueue = newLucene<ScorerDocQueue>(nrScorers);
    for (Collection<ScorerPtr>::iterator se = subScorers.begin(); se != subScorers.end(); ++se) {
        if ((*se)->nextDoc() != NO_MORE_DOCS) {
            scorerDocQueue->insert(*se);
        }
    }
}

void DisjunctionSumScorer::score(const CollectorPtr& collector) {
    collector->setScorer(shared_from_this());
    while (nextDoc() != NO_MORE_DOCS) {
        collector->collect(currentDoc);
    }
}

bool DisjunctionSumScorer::score(const CollectorPtr& collector, int32_t max, int32_t firstDocID) {
    // firstDocID is ignored since nextDoc() sets 'currentDoc'
    collector->setScorer(shared_from_this());
    while (currentDoc < max) {
        collector->collect(currentDoc);
        if (nextDoc() == NO_MORE_DOCS) {
            return false;
        }
    }
    return true;
}

int32_t DisjunctionSumScorer::nextDoc() {
    if (scorerDocQueue->size() < minimumNrMatchers || !advanceAfterCurrent()) {
        currentDoc = NO_MORE_DOCS;
    }
    return currentDoc;
}

bool DisjunctionSumScorer::advanceAfterCurrent() {
    do { // repeat until minimum nr of matchers
        currentDoc = scorerDocQueue->topDoc();
        currentScore = scorerDocQueue->topScore();
        _nrMatchers = 1;
        do { // Until all subscorers are after currentDoc
            if (!scorerDocQueue->topNextAndAdjustElsePop()) {
                if (scorerDocQueue->size() == 0) {
                    break;    // nothing more to advance, check for last match.
                }
            }
            if (scorerDocQueue->topDoc() != currentDoc) {
                break;    // All remaining subscorers are after currentDoc.
            }
            currentScore += scorerDocQueue->topScore();
            ++_nrMatchers;
        } while (true);

        if (_nrMatchers >= minimumNrMatchers) {
            return true;
        } else if (scorerDocQueue->size() < minimumNrMatchers) {
            return false;
        }
    } while (true);
}

double DisjunctionSumScorer::score() {
    return currentScore;
}

int32_t DisjunctionSumScorer::docID() {
    return currentDoc;
}

int32_t DisjunctionSumScorer::nrMatchers() {
    return _nrMatchers;
}

int32_t DisjunctionSumScorer::advance(int32_t target) {
    if (scorerDocQueue->size() < minimumNrMatchers) {
        currentDoc = NO_MORE_DOCS;
        return currentDoc;
    }
    if (target <= currentDoc) {
        return currentDoc;
    }
    do {
        if (scorerDocQueue->topDoc() >= target) {
            if (!advanceAfterCurrent()) {
                currentDoc = NO_MORE_DOCS;
            }
            return currentDoc;
        } else if (!scorerDocQueue->topSkipToAndAdjustElsePop(target)) {
            if (scorerDocQueue->size() < minimumNrMatchers) {
                currentDoc = NO_MORE_DOCS;
                return currentDoc;
            }
        }
    } while (true);
}

}
