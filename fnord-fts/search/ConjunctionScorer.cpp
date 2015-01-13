/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/ConjunctionScorer.h"
#include "fnord-fts/search/Similarity.h"

namespace Lucene {

struct lessScorerDocId {
    inline bool operator()(const ScorerPtr& first, const ScorerPtr& second) const {
        return (first->docID() < second->docID());
    }
};

ConjunctionScorer::ConjunctionScorer(const SimilarityPtr& similarity, Collection<ScorerPtr> scorers) : Scorer(similarity) {
    this->lastDoc = -1;
    this->scorers = scorers;
    this->coord = similarity->coord(scorers.size(), scorers.size());

    for (Collection<ScorerPtr>::iterator scorer = scorers.begin(); scorer != scorers.end(); ++scorer) {
        if ((*scorer)->nextDoc() == NO_MORE_DOCS) {
            // If even one of the sub-scorers does not have any documents, this scorer should not attempt
            // to do any more work.
            lastDoc = NO_MORE_DOCS;
            return;
        }
    }

    // Sort the array the first time...
    // We don't need to sort the array in any future calls because we know it will already start off
    // sorted (all scorers on same doc).
    std::sort(scorers.begin(), scorers.end(), lessScorerDocId());

    // NOTE: doNext() must be called before the re-sorting of the array later on.  The reason is this:
    // assume there are 5 scorers, whose first docs are 1, 2, 3, 5, 5 respectively. Sorting (above) leaves
    // the array as is.  Calling doNext() here advances all the first scorers to 5 (or a larger doc ID
    // they all agree on).
    // However, if we re-sort before doNext() is called, the order will be 5, 3, 2, 1, 5 and then doNext()
    // will stop immediately, since the first scorer's docs equals the last one. So the invariant that after
    // calling doNext() all scorers are on the same doc ID is broken.
    if (doNext() == NO_MORE_DOCS) {
        // The scorers did not agree on any document.
        lastDoc = NO_MORE_DOCS;
        return;
    }

    // If first-time skip distance is any predictor of scorer sparseness, then we should always try to skip
    // first on those scorers.  Keep last scorer in it's last place (it will be the first to be skipped on),
    // but reverse all of the others so that they will be skipped on in order of original high skip.
    int32_t end = scorers.size() - 1;
    int32_t max = end >> 1;
    for (int32_t i = 0; i < max; ++i) {
        ScorerPtr tmp(scorers[i]);
        int32_t idx = end - i - 1;
        scorers[i] = scorers[idx];
        scorers[idx] = tmp;
    }
}

ConjunctionScorer::~ConjunctionScorer() {
}

int32_t ConjunctionScorer::doNext() {
    int32_t first = 0;
    int32_t doc = scorers[scorers.size() - 1]->docID();
    ScorerPtr firstScorer;
    while ((firstScorer = scorers[first])->docID() < doc) {
        doc = firstScorer->advance(doc);
        first = first == scorers.size() - 1 ? 0 : first + 1;
    }
    return doc;
}

int32_t ConjunctionScorer::advance(int32_t target) {
    if (lastDoc == NO_MORE_DOCS) {
        return lastDoc;
    } else if (scorers[(scorers.size() - 1)]->docID() < target) {
        scorers[(scorers.size() - 1)]->advance(target);
    }
    lastDoc = doNext();
    return lastDoc;
}

int32_t ConjunctionScorer::docID() {
    return lastDoc;
}

int32_t ConjunctionScorer::nextDoc() {
    if (lastDoc == NO_MORE_DOCS) {
        return lastDoc;
    } else if (lastDoc == -1) {
        lastDoc = scorers[scorers.size() - 1]->docID();
        return lastDoc;
    }
    scorers[(scorers.size() - 1)]->nextDoc();
    lastDoc = doNext();
    return lastDoc;
}

double ConjunctionScorer::score() {
    double sum = 0.0;
    for (Collection<ScorerPtr>::iterator scorer = scorers.begin(); scorer != scorers.end(); ++scorer) {
        sum += (*scorer)->score();
    }
    return sum * coord;
}

}
