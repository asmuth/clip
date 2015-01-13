/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentMergeQueue.h"
#include "SegmentMergeInfo.h"

namespace Lucene {

SegmentMergeQueue::SegmentMergeQueue(int32_t size) : PriorityQueue<SegmentMergeInfoPtr>(size) {
}

SegmentMergeQueue::~SegmentMergeQueue() {
}

void SegmentMergeQueue::close() {
    while (top()) {
        pop()->close();
    }
}

bool SegmentMergeQueue::lessThan(const SegmentMergeInfoPtr& first, const SegmentMergeInfoPtr& second) {
    int32_t comparison = first->term->compareTo(second->term);
    return comparison == 0 ? (first->base < second->base) : (comparison < 0);
}

}
