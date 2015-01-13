/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONCURRENTMERGESCHEDULER_H
#define _CONCURRENTMERGESCHEDULER_H

#include "LuceneThread.h"

namespace Lucene {

class MergeThread : public LuceneThread {
public:
    MergeThread(const ConcurrentMergeSchedulerPtr& merger, const IndexWriterPtr& writer, const OneMergePtr& startMerge);
    virtual ~MergeThread();

    LUCENE_CLASS(MergeThread);

protected:
    ConcurrentMergeSchedulerWeakPtr _merger;
    IndexWriterWeakPtr _writer;
    OneMergePtr startMerge;
    OneMergePtr runningMerge;

public:
    void setRunningMerge(const OneMergePtr& merge);
    OneMergePtr getRunningMerge();
    void setThreadPriority(int32_t pri);
    virtual void run();
};

}

#endif
