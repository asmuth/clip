/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef HITQUEUEBASE_H
#define HITQUEUEBASE_H

#include "fnord-fts/util/PriorityQueue.h"

namespace fnord {
namespace fts {

class HitQueueBase : public LuceneObject {
public:
    HitQueueBase(int32_t size);
    virtual ~HitQueueBase();

    LUCENE_CLASS(HitQueueBase);

public:
    virtual ScoreDocPtr add(const ScoreDocPtr& scoreDoc);
    virtual ScoreDocPtr addOverflow(const ScoreDocPtr& scoreDoc);
    virtual ScoreDocPtr top();
    virtual ScoreDocPtr pop();
    virtual ScoreDocPtr updateTop();
    virtual int32_t size();
    virtual bool empty();
    virtual void clear();

protected:
    PriorityQueueScoreDocsPtr queue;
    int32_t queueSize;

public:
    virtual void initialize();

protected:
    virtual bool lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second) = 0;
    virtual ScoreDocPtr getSentinelObject();

    friend class PriorityQueueScoreDocs;
};

class PriorityQueueScoreDocs : public PriorityQueue<ScoreDocPtr> {
public:
    PriorityQueueScoreDocs(const HitQueueBasePtr& hitQueue, int32_t size);
    virtual ~PriorityQueueScoreDocs();

    LUCENE_CLASS(PriorityQueueScoreDocs);

protected:
    HitQueueBaseWeakPtr _hitQueue;

protected:
    virtual bool lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second);
    virtual ScoreDocPtr getSentinelObject();
};

}

}
#endif
