/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/thread/thread.hpp>
#include "LuceneThread.h"

namespace Lucene {

#if defined(_WIN32) || defined(_WIN64)
const int32_t LuceneThread::MAX_THREAD_PRIORITY = THREAD_PRIORITY_HIGHEST;
const int32_t LuceneThread::NORM_THREAD_PRIORITY = THREAD_PRIORITY_NORMAL;
const int32_t LuceneThread::MIN_THREAD_PRIORITY = THREAD_PRIORITY_LOWEST;
#else
const int32_t LuceneThread::MAX_THREAD_PRIORITY = 2;
const int32_t LuceneThread::NORM_THREAD_PRIORITY = 0;
const int32_t LuceneThread::MIN_THREAD_PRIORITY = -2;
#endif

LuceneThread::LuceneThread() {
    running = false;
}

LuceneThread::~LuceneThread() {
}

void LuceneThread::start() {
    setRunning(false);
    thread = newInstance<boost::thread>(LuceneThread::runThread, this);
    setRunning(true);
}

void LuceneThread::runThread(LuceneThread* thread) {
    LuceneThreadPtr threadObject(thread->shared_from_this());
    try {
        threadObject->run();
    } catch (...) {
    }
    threadObject->setRunning(false);
    threadObject.reset();
}

void LuceneThread::setRunning(bool running) {
    SyncLock syncLock(this);
    this->running = running;
}

bool LuceneThread::isRunning() {
    SyncLock syncLock(this);
    return running;
}

bool LuceneThread::isAlive() {
    return (thread && isRunning());
}

void LuceneThread::setPriority(int32_t priority) {
#if defined(_WIN32) || defined(_WIN64)
    if (thread) {
        SetThreadPriority(thread->native_handle(), priority);
    }
#endif
}

int32_t LuceneThread::getPriority() {
#if defined(_WIN32) || defined(_WIN64)
    return thread ? GetThreadPriority(thread->native_handle()) : NORM_THREAD_PRIORITY;
#else
    return NORM_THREAD_PRIORITY;
#endif
}

void LuceneThread::yield() {
    if (thread) {
        thread->yield();
    }
}

bool LuceneThread::join(int32_t timeout) {
    while (isAlive() && !thread->timed_join(boost::posix_time::milliseconds(timeout))) {
        if (timeout != 0) {
            return false;
        }
        if (thread->timed_join(boost::posix_time::milliseconds(10))) {
            return true;
        }
    }
    return true;
}

int64_t LuceneThread::currentId() {
#if defined(_WIN32) || defined(_WIN64)
    return (int64_t)GetCurrentThreadId();
#else
    return (int64_t)pthread_self();
#endif
}

void LuceneThread::threadSleep(int32_t time) {
    boost::this_thread::sleep(boost::posix_time::milliseconds(time));
}

void LuceneThread::threadYield() {
    boost::this_thread::yield();
}

}
