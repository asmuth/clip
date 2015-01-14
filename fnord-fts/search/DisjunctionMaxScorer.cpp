/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/DisjunctionMaxScorer.h"

namespace Lucene {

DisjunctionMaxScorer::DisjunctionMaxScorer(double tieBreakerMultiplier, const SimilarityPtr& similarity, Collection<ScorerPtr> subScorers, int32_t numScorers) : Scorer(similarity) {
    this->doc = -1;
    this->tieBreakerMultiplier = tieBreakerMultiplier;

    // The passed subScorers array includes only scorers which have documents (DisjunctionMaxQuery takes care
    // of that), and their nextDoc() was already called.
    this->subScorers = subScorers;
    this->numScorers = numScorers;

    heapify();
}

DisjunctionMaxScorer::~DisjunctionMaxScorer() {
}

int32_t DisjunctionMaxScorer::nextDoc() {
    if (numScorers == 0) {
        doc = NO_MORE_DOCS;
        return doc;
    }
    while (subScorers[0]->docID() == doc) {
        if (subScorers[0]->nextDoc() != NO_MORE_DOCS) {
            heapAdjust(0);
        } else {
            heapRemoveRoot();
            if (numScorers == 0) {
                doc = NO_MORE_DOCS;
                return doc;
            }
        }
    }

    doc = subScorers[0]->docID();
    return doc;
}

int32_t DisjunctionMaxScorer::docID() {
    return doc;
}

double DisjunctionMaxScorer::score() {
    int32_t doc = subScorers[0]->docID();
    Collection<double> sum(newCollection<double>(subScorers[0]->score()));
    Collection<double> max(Collection<double>::newInstance(sum.begin(), sum.end()));
    int32_t size = numScorers;
    scoreAll(1, size, doc, sum, max);
    scoreAll(2, size, doc, sum, max);
    return max[0] + (sum[0] - max[0]) * tieBreakerMultiplier;
}

void DisjunctionMaxScorer::scoreAll(int32_t root, int32_t size, int32_t doc, Collection<double> sum, Collection<double> max) {
    if (root < size && subScorers[root]->docID() == doc) {
        double sub = subScorers[root]->score();
        sum[0] += sub;
        max[0] = std::max(max[0], sub);
        scoreAll((root << 1) + 1, size, doc, sum, max);
        scoreAll((root << 1) + 2, size, doc, sum, max);
    }
}

int32_t DisjunctionMaxScorer::advance(int32_t target) {
    if (numScorers == 0) {
        doc = NO_MORE_DOCS;
        return doc;
    }
    while (subScorers[0]->docID() < target) {
        if (subScorers[0]->advance(target) != NO_MORE_DOCS) {
            heapAdjust(0);
        } else {
            heapRemoveRoot();
            if (numScorers == 0) {
                doc = NO_MORE_DOCS;
                return doc;
            }
        }
    }
    doc = subScorers[0]->docID();
    return doc;
}

void DisjunctionMaxScorer::heapify() {
    for (int32_t i = (numScorers >> 1) - 1; i >= 0; --i) {
        heapAdjust(i);
    }
}

void DisjunctionMaxScorer::heapAdjust(int32_t root) {
    ScorerPtr scorer(subScorers[root]);
    int32_t doc = scorer->docID();
    int32_t i = root;
    while (i <= (numScorers >> 1) - 1) {
        int32_t lchild = (i << 1) + 1;
        ScorerPtr lscorer(subScorers[lchild]);
        int32_t ldoc = lscorer->docID();
        int32_t rdoc = INT_MAX;
        int32_t rchild = (i << 1) + 2;
        ScorerPtr rscorer;
        if (rchild < numScorers) {
            rscorer = subScorers[rchild];
            rdoc = rscorer->docID();
        }
        if (ldoc < doc) {
            if (rdoc < ldoc) {
                subScorers[i] = rscorer;
                subScorers[rchild] = scorer;
                i = rchild;
            } else {
                subScorers[i] = lscorer;
                subScorers[lchild] = scorer;
                i = lchild;
            }
        } else if (rdoc < doc) {
            subScorers[i] = rscorer;
            subScorers[rchild] = scorer;
            i = rchild;
        } else {
            return;
        }
    }
}

void DisjunctionMaxScorer::heapRemoveRoot() {
    if (numScorers == 1) {
        subScorers[0].reset();
        numScorers = 0;
    } else {
        subScorers[0] = subScorers[numScorers - 1];
        subScorers[numScorers - 1].reset();
        --numScorers;
        heapAdjust(0);
    }
}

}
