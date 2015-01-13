/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENETHREAD_H
#define LUCENETHREAD_H

#include "LuceneObject.h"

namespace Lucene {

/// Lucene thread container.
///
/// It seems there are major issues with using boost::thread::id under Windows.
/// After many hours of debugging and trying various strategies, I was unable to fix an
/// occasional crash whereby boost::thread::thread_data was being deleted prematurely.
///
/// This problem is most visible when running the AtomicUpdateTest test suite.
///
/// Therefore, I now uniquely identify threads by their native id.
class LuceneThread : public LuceneObject {
public:
    LuceneThread();
    virtual ~LuceneThread();

    LUCENE_CLASS(LuceneThread);

public:
    static const int32_t MAX_THREAD_PRIORITY;
    static const int32_t NORM_THREAD_PRIORITY;
    static const int32_t MIN_THREAD_PRIORITY;

protected:
    threadPtr thread;

    /// Flag to indicate running thread.
    /// @see #isAlive
    bool running;

public:
    /// start thread see {@link #run}.
    virtual void start();

    /// return whether thread is current running.
    virtual bool isAlive();

    /// set running thread priority.
    virtual void setPriority(int32_t priority);

    /// return running thread priority.
    virtual int32_t getPriority();

    /// wait for thread to finish using an optional timeout.
    virtual bool join(int32_t timeout = 0);

    /// causes the currently executing thread object to temporarily pause and allow other threads to execute.
    virtual void yield();

    /// override to provide the body of the thread.
    virtual void run() = 0;

    /// Return representation of current execution thread.
    static int64_t currentId();

    /// Suspends current execution thread for a given time.
    static void threadSleep(int32_t time);

    /// Yield current execution thread.
    static void threadYield();

protected:
    /// set thread running state.
    void setRunning(bool running);

    /// return thread running state.
    bool isRunning();

    /// function that controls the lifetime of the running thread.
    static void runThread(LuceneThread* thread);
};

}

#endif
