/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ConcurrentMergeScheduler.h"
#include "_ConcurrentMergeScheduler.h"
#include "IndexWriter.h"
#include "TestPoint.h"
#include "StringUtils.h"

namespace Lucene {

Collection<ConcurrentMergeSchedulerPtr> ConcurrentMergeScheduler::allInstances;
bool ConcurrentMergeScheduler::anyExceptions = false;

ConcurrentMergeScheduler::ConcurrentMergeScheduler() {
    mergeThreadPriority = -1;
    mergeThreads = SetMergeThread::newInstance();
    maxThreadCount = 1;
    suppressExceptions = false;
    closed = false;
}

ConcurrentMergeScheduler::~ConcurrentMergeScheduler() {
}

void ConcurrentMergeScheduler::initialize() {
    // Only for testing
    if (allInstances) {
        addMyself();
    }
}

void ConcurrentMergeScheduler::setMaxThreadCount(int32_t count) {
    if (count < 1) {
        boost::throw_exception(IllegalArgumentException(L"count should be at least 1"));
    }
    maxThreadCount = count;
}

int32_t ConcurrentMergeScheduler::getMaxThreadCount() {
    return maxThreadCount;
}

int32_t ConcurrentMergeScheduler::getMergeThreadPriority() {
    SyncLock syncLock(this);
    initMergeThreadPriority();
    return mergeThreadPriority;
}

void ConcurrentMergeScheduler::setMergeThreadPriority(int32_t pri) {
    SyncLock syncLock(this);
    if (pri > LuceneThread::MAX_THREAD_PRIORITY || pri < LuceneThread::MIN_THREAD_PRIORITY) {
        boost::throw_exception(IllegalArgumentException(L"priority must be in range " + StringUtils::toString(LuceneThread::MIN_THREAD_PRIORITY) +
                               L" .. " + StringUtils::toString(LuceneThread::MAX_THREAD_PRIORITY) + L" inclusive"));
    }
    mergeThreadPriority = pri;

    for (SetMergeThread::iterator merge = mergeThreads.begin(); merge != mergeThreads.end(); ++merge) {
        (*merge)->setThreadPriority(pri);
    }
}

bool ConcurrentMergeScheduler::verbose() {
    return (!_writer.expired() && IndexWriterPtr(_writer)->verbose());
}

void ConcurrentMergeScheduler::message(const String& message) {
    if (verbose() && !_writer.expired()) {
        IndexWriterPtr(_writer)->message(L"CMS: " + message);
    }
}

void ConcurrentMergeScheduler::initMergeThreadPriority() {
    SyncLock syncLock(this);
    if (mergeThreadPriority == -1) {
        // Default to slightly higher priority than our calling thread
        mergeThreadPriority = std::min(LuceneThread::NORM_THREAD_PRIORITY + 1, LuceneThread::MAX_THREAD_PRIORITY);
    }
}

void ConcurrentMergeScheduler::close() {
    sync();
    closed = true;
}

void ConcurrentMergeScheduler::sync() {
    SyncLock syncLock(this);
    while (mergeThreadCount() > 0) {
        message(L"now wait for threads; currently " + StringUtils::toString(mergeThreads.size()) + L" still running");
        wait(1000);
    }
    mergeThreads.clear();
}

int32_t ConcurrentMergeScheduler::mergeThreadCount() {
    SyncLock syncLock(this);
    int32_t count = 0;
    for (SetMergeThread::iterator merge = mergeThreads.begin(); merge != mergeThreads.end(); ++merge) {
        if ((*merge)->isAlive()) {
            ++count;
        }
    }
    return count;
}

void ConcurrentMergeScheduler::merge(const IndexWriterPtr& writer) {
    BOOST_ASSERT(!writer->holdsLock());

    this->_writer = writer;

    initMergeThreadPriority();

    dir = writer->getDirectory();

    // First, quickly run through the newly proposed merges and add any orthogonal merges (ie a merge not
    // involving segments already pending to be merged) to the queue.  If we are way behind on merging,
    // many of these newly proposed merges will likely already be registered.
    message(L"now merge");
    message(L"  index: " + writer->segString());

    // Iterate, pulling from the IndexWriter's queue of pending merges, until it's empty
    while (true) {
        OneMergePtr merge(writer->getNextMerge());
        if (!merge) {
            message(L"  no more merges pending; now return");
            return;
        }

        // We do this with the primary thread to keep deterministic assignment of segment names
        writer->mergeInit(merge);

        bool success = false;
        LuceneException finally;
        try {
            SyncLock syncLock(this);
            MergeThreadPtr merger;
            while (mergeThreadCount() >= maxThreadCount) {
                message(L"    too many merge threads running; stalling...");
                wait(1000);
            }

            message(L"  consider merge " + merge->segString(dir));

            BOOST_ASSERT(mergeThreadCount() < maxThreadCount);

            // OK to spawn a new merge thread to handle this merge
            merger = getMergeThread(writer, merge);
            mergeThreads.add(merger);
            message(L"    launch new thread");

            merger->start();
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            writer->mergeFinish(merge);
        }
        finally.throwException();
    }
}

void ConcurrentMergeScheduler::doMerge(const OneMergePtr& merge) {
    TestScope testScope(L"ConcurrentMergeScheduler", L"doMerge");
    IndexWriterPtr(_writer)->merge(merge);
}

MergeThreadPtr ConcurrentMergeScheduler::getMergeThread(const IndexWriterPtr& writer, const OneMergePtr& merge) {
    SyncLock syncLock(this);
    MergeThreadPtr thread(newLucene<MergeThread>(shared_from_this(), writer, merge));
    thread->setThreadPriority(mergeThreadPriority);
    return thread;
}

void ConcurrentMergeScheduler::handleMergeException(const LuceneException& exc) {
    // When an exception is hit during merge, IndexWriter removes any partial files and then
    // allows another merge to run.  If whatever caused the error is not transient then the
    // exception will keep happening, so, we sleep here to avoid saturating CPU in such cases
    LuceneThread::threadSleep(250); // pause 250 msec
    boost::throw_exception(MergeException());
}

bool ConcurrentMergeScheduler::anyUnhandledExceptions() {
    if (!allInstances) {
        boost::throw_exception(RuntimeException(L"setTestMode() was not called"));
    }
    SyncLock instancesLock(&allInstances);
    for (Collection<ConcurrentMergeSchedulerPtr>::iterator instance = allInstances.begin(); instance != allInstances.end(); ++instance) {
        (*instance)->sync();
    }
    bool v = anyExceptions;
    anyExceptions = false;
    return v;
}

void ConcurrentMergeScheduler::clearUnhandledExceptions() {
    SyncLock instancesLock(&allInstances);
    anyExceptions = false;
}

void ConcurrentMergeScheduler::addMyself() {
    SyncLock instancesLock(&allInstances);
    int32_t size = allInstances.size();
    int32_t upto = 0;
    for (int32_t i = 0; i < size; ++i) {
        ConcurrentMergeSchedulerPtr other(allInstances[i]);
        if (!(other->closed && other->mergeThreadCount() == 0)) {
            // Keep this one for now: it still has threads or may spawn new threads
            allInstances[upto++] = other;
        }

        allInstances.remove(allInstances.begin() + upto, allInstances.end());
        allInstances.add(shared_from_this());
    }
}

void ConcurrentMergeScheduler::setSuppressExceptions() {
    suppressExceptions = true;
}

void ConcurrentMergeScheduler::clearSuppressExceptions() {
    suppressExceptions = false;
}

void ConcurrentMergeScheduler::setTestMode() {
    allInstances = Collection<ConcurrentMergeSchedulerPtr>::newInstance();
}

MergeThread::MergeThread(const ConcurrentMergeSchedulerPtr& merger, const IndexWriterPtr& writer, const OneMergePtr& startMerge) {
    this->_merger = merger;
    this->_writer = writer;
    this->startMerge = startMerge;
}

MergeThread::~MergeThread() {
}

void MergeThread::setRunningMerge(const OneMergePtr& merge) {
    ConcurrentMergeSchedulerPtr merger(_merger);
    SyncLock syncLock(merger);
    runningMerge = merge;
}

OneMergePtr MergeThread::getRunningMerge() {
    ConcurrentMergeSchedulerPtr merger(_merger);
    SyncLock syncLock(merger);
    return runningMerge;
}

void MergeThread::setThreadPriority(int32_t pri) {
    try {
        setPriority(pri);
    } catch (...) {
    }
}

void MergeThread::run() {
    // First time through the while loop we do the merge that we were started with
    OneMergePtr merge(this->startMerge);
    ConcurrentMergeSchedulerPtr merger(_merger);

    LuceneException finally;
    try {
        merger->message(L"  merge thread: start");
        IndexWriterPtr writer(_writer);

        while (true) {
            setRunningMerge(merge);
            merger->doMerge(merge);

            // Subsequent times through the loop we do any new merge that writer says is necessary
            merge = writer->getNextMerge();
            if (merge) {
                writer->mergeInit(merge);
                merger->message(L"  merge thread: do another merge " + merge->segString(merger->dir));
            } else {
                break;
            }
        }

        merger->message(L"  merge thread: done");
    } catch (MergeAbortedException&) {
        // Ignore the exception if it was due to abort
    } catch (LuceneException& e) {
        if (!merger->suppressExceptions) {
            // suppressExceptions is normally only set during testing.
            merger->anyExceptions = true;
            merger->handleMergeException(e);
        } else {
            finally = e;
        }
    }

    {
        SyncLock syncLock(merger);
        merger->notifyAll();

        bool removed = merger->mergeThreads.remove(shared_from_this());
        BOOST_ASSERT(removed);
    }
    finally.throwException();
}

}
