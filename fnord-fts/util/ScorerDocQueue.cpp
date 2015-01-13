/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ScorerDocQueue.h"
#include "_ScorerDocQueue.h"
#include "Scorer.h"
#include "MiscUtils.h"

namespace Lucene {

ScorerDocQueue::ScorerDocQueue(int32_t maxSize) {
    this->_size = 0;
    int32_t heapSize = maxSize + 1;
    heap = Collection<HeapedScorerDocPtr>::newInstance(heapSize);
    this->maxSize = maxSize;
    topHSD = heap[1]; // initially null
}

ScorerDocQueue::~ScorerDocQueue() {
}

void ScorerDocQueue::put(const ScorerPtr& scorer) {
    heap[++_size] = newLucene<HeapedScorerDoc>(scorer);
    upHeap();
}

bool ScorerDocQueue::insert(const ScorerPtr& scorer) {
    if (_size < maxSize) {
        put(scorer);
        return true;
    } else {
        int32_t docNr = scorer->docID();
        if ((_size > 0) && (!(docNr < topHSD->doc))) { // heap[1] is top()
            heap[1] = newLucene<HeapedScorerDoc>(scorer, docNr);
            downHeap();
            return true;
        } else {
            return false;
        }
    }
}

ScorerPtr ScorerDocQueue::top() {
    return topHSD->scorer;
}

int32_t ScorerDocQueue::topDoc() {
    return topHSD->doc;
}

double ScorerDocQueue::topScore() {
    return topHSD->scorer->score();
}

bool ScorerDocQueue::topNextAndAdjustElsePop() {
    return checkAdjustElsePop(topHSD->scorer->nextDoc() != DocIdSetIterator::NO_MORE_DOCS);
}

bool ScorerDocQueue::topSkipToAndAdjustElsePop(int32_t target) {
    return checkAdjustElsePop(topHSD->scorer->advance(target) != DocIdSetIterator::NO_MORE_DOCS);
}

bool ScorerDocQueue::checkAdjustElsePop(bool cond) {
    if (cond) { // see also adjustTop
        topHSD->doc = topHSD->scorer->docID();
    } else { // see also popNoResult
        heap[1] = heap[_size]; // move last to first
        heap[_size--].reset();
    }
    downHeap();
    return cond;
}

ScorerPtr ScorerDocQueue::pop() {
    ScorerPtr result(topHSD->scorer);
    popNoResult();
    return result;
}

void ScorerDocQueue::popNoResult() {
    heap[1] = heap[_size]; // move last to first
    heap[_size--].reset();
    downHeap(); // adjust heap
}

void ScorerDocQueue::adjustTop() {
    topHSD->adjust();
    downHeap();
}

int32_t ScorerDocQueue::size() {
    return _size;
}

void ScorerDocQueue::clear() {
    for (int32_t i = 0; i <= _size; ++i) {
        heap[i].reset();
    }
    _size = 0;
}

void ScorerDocQueue::upHeap() {
    int32_t i = _size;
    HeapedScorerDocPtr node(heap[i]); // save bottom node
    int32_t j = MiscUtils::unsignedShift(i, 1);
    while ((j > 0) && (node->doc < heap[j]->doc)) {
        heap[i] = heap[j]; // shift parents down
        i = j;
        j = MiscUtils::unsignedShift(j, 1);
    }
    heap[i] = node; // install saved node
    topHSD = heap[1];
}

void ScorerDocQueue::downHeap() {
    int32_t i = 1;
    HeapedScorerDocPtr node(heap[i]); // save top node
    int32_t j = i << 1; // find smaller child
    int32_t k = j + 1;
    if ((k <= _size) && (heap[k]->doc < heap[j]->doc)) {
        j = k;
    }
    while ((j <= _size) && (heap[j]->doc < node->doc)) {
        heap[i] = heap[j]; // shift up child
        i = j;
        j = i << 1;
        k = j + 1;
        if (k <= _size && (heap[k]->doc < heap[j]->doc)) {
            j = k;
        }
    }
    heap[i] = node; // install saved node
    topHSD = heap[1];
}

HeapedScorerDoc::HeapedScorerDoc(const ScorerPtr& scorer) {
    this->scorer = scorer;
    this->doc = scorer->docID();
}

HeapedScorerDoc::HeapedScorerDoc(const ScorerPtr& scorer, int32_t doc) {
    this->scorer = scorer;
    this->doc = doc;
}

HeapedScorerDoc::~HeapedScorerDoc() {
}

void HeapedScorerDoc::adjust() {
    doc = scorer->docID();
}

}
