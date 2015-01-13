/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/HitQueueBase.h"
#include "fnord-fts/search/ScoreDoc.h"

namespace Lucene {

HitQueueBase::HitQueueBase(int32_t size) {
    queueSize = size;
}

HitQueueBase::~HitQueueBase() {
}

void HitQueueBase::initialize() {
    queue = newLucene<PriorityQueueScoreDocs>(shared_from_this(), queueSize);
}

ScoreDocPtr HitQueueBase::add(const ScoreDocPtr& scoreDoc) {
    return queue->add(scoreDoc);
}

ScoreDocPtr HitQueueBase::addOverflow(const ScoreDocPtr& scoreDoc) {
    return queue->addOverflow(scoreDoc);
}

ScoreDocPtr HitQueueBase::top() {
    return queue->top();
}

ScoreDocPtr HitQueueBase::pop() {
    return queue->pop();
}

ScoreDocPtr HitQueueBase::updateTop() {
    return queue->updateTop();
}

int32_t HitQueueBase::size() {
    return queue->size();
}

bool HitQueueBase::empty() {
    return queue->empty();
}

void HitQueueBase::clear() {
    queue->clear();
}

ScoreDocPtr HitQueueBase::getSentinelObject() {
    return ScoreDocPtr();
}

PriorityQueueScoreDocs::PriorityQueueScoreDocs(const HitQueueBasePtr& hitQueue, int32_t size) : PriorityQueue<ScoreDocPtr>(size) {
    _hitQueue = hitQueue;
}

PriorityQueueScoreDocs::~PriorityQueueScoreDocs() {
}

bool PriorityQueueScoreDocs::lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second) {
    return HitQueueBasePtr(_hitQueue)->lessThan(first, second);
}

ScoreDocPtr PriorityQueueScoreDocs::getSentinelObject() {
    return HitQueueBasePtr(_hitQueue)->getSentinelObject();
}

}
