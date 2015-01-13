/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/HitQueue.h"
#include "fnord-fts/search/ScoreDoc.h"

namespace Lucene {

HitQueue::HitQueue(int32_t size, bool prePopulate) : HitQueueBase(size) {
    this->prePopulate = prePopulate;
}

HitQueue::~HitQueue() {
}

bool HitQueue::lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second) {
    if (first->score == second->score) {
        return (first->doc > second->doc);
    } else {
        return (first->score < second->score);
    }
}

ScoreDocPtr HitQueue::getSentinelObject() {
    // Always set the doc Id to MAX_VALUE so that it won't be favored by lessThan. This generally should
    // not happen since if score is not NEG_INF, TopScoreDocCollector will always add the object to the queue.
    return !prePopulate ? ScoreDocPtr() : newLucene<ScoreDoc>(INT_MAX, -std::numeric_limits<double>::infinity());
}

}
