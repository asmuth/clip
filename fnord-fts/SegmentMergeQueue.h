/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTMERGEQUEUE_H
#define SEGMENTMERGEQUEUE_H

#include "PriorityQueue.h"

namespace Lucene {

class SegmentMergeQueue : public PriorityQueue<SegmentMergeInfoPtr> {
public:
    SegmentMergeQueue(int32_t size);
    virtual ~SegmentMergeQueue();

    LUCENE_CLASS(SegmentMergeQueue);

public:
    void close();

protected:
    virtual bool lessThan(const SegmentMergeInfoPtr& first, const SegmentMergeInfoPtr& second);
};

}

#endif
