/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/ScoreCachingWrappingScorer.h"

namespace Lucene {

ScoreCachingWrappingScorer::ScoreCachingWrappingScorer(const ScorerPtr& scorer) : Scorer(scorer->getSimilarity()) {
    this->curDoc = -1;
    this->curScore = 0.0;
    this->_scorer = scorer;
}

ScoreCachingWrappingScorer::~ScoreCachingWrappingScorer() {
}

bool ScoreCachingWrappingScorer::score(const CollectorPtr& collector, int32_t max, int32_t firstDocID) {
    return ScorerPtr(_scorer)->score(collector, max, firstDocID);
}

SimilarityPtr ScoreCachingWrappingScorer::getSimilarity() {
    return ScorerPtr(_scorer)->getSimilarity();
}

double ScoreCachingWrappingScorer::score() {
    ScorerPtr scorer(_scorer);
    int32_t doc = scorer->docID();
    if (doc != curDoc) {
        curScore = scorer->score();
        curDoc = doc;
    }
    return curScore;
}

int32_t ScoreCachingWrappingScorer::docID() {
    return ScorerPtr(_scorer)->docID();
}

int32_t ScoreCachingWrappingScorer::nextDoc() {
    return ScorerPtr(_scorer)->nextDoc();
}

void ScoreCachingWrappingScorer::score(const CollectorPtr& collector) {
    ScorerPtr(_scorer)->score(collector);
}

int32_t ScoreCachingWrappingScorer::advance(int32_t target) {
    return ScorerPtr(_scorer)->advance(target);
}

}
