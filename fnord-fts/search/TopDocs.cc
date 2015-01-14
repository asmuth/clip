/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/TopDocs.h"

namespace fnord {
namespace fts {

TopDocs::TopDocs(int32_t totalHits, Collection<ScoreDocPtr> scoreDocs) {
    this->totalHits = totalHits;
    this->scoreDocs = scoreDocs;
    this->maxScore = std::numeric_limits<double>::quiet_NaN();
}

TopDocs::TopDocs(int32_t totalHits, Collection<ScoreDocPtr> scoreDocs, double maxScore) {
    this->totalHits = totalHits;
    this->scoreDocs = scoreDocs;
    this->maxScore = maxScore;
}

TopDocs::~TopDocs() {
}

double TopDocs::getMaxScore() {
    return maxScore;
}

void TopDocs::setMaxScore(double maxScore) {
    this->maxScore = maxScore;
}

}

}
