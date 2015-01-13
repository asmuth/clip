/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _TIMELIMITINGCOLLECTOR_H
#define _TIMELIMITINGCOLLECTOR_H

#include "LuceneThread.h"

namespace Lucene {

class TimerThread : public LuceneThread {
public:
    TimerThread();
    virtual ~TimerThread();

    LUCENE_CLASS(TimerThread);

protected:
    int64_t time;
    bool _stopThread;

public:
    virtual void start();
    virtual void run();

    /// Get the timer value in milliseconds.
    int64_t getMilliseconds();

    /// Stop timer thread.
    void stopThread();
};

}

#endif
