/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/Scorer.h"
#include "fnord-fts/search/Collector.h"

namespace Lucene {

Scorer::Scorer(const SimilarityPtr& similarity) {
    this->similarity = similarity;
}

Scorer::~Scorer() {
}

SimilarityPtr Scorer::getSimilarity() {
    return similarity;
}

void Scorer::score(const CollectorPtr& collector) {
    collector->setScorer(shared_from_this());
    int32_t doc;
    while ((doc = nextDoc()) != NO_MORE_DOCS) {
        collector->collect(doc);
    }
}

bool Scorer::score(const CollectorPtr& collector, int32_t max, int32_t firstDocID) {
    collector->setScorer(shared_from_this());
    int32_t doc = firstDocID;
    while (doc < max) {
        collector->collect(doc);
        doc = nextDoc();
    }
    return (doc != NO_MORE_DOCS);
}

}
