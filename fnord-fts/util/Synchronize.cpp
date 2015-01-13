/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/thread/mutex.hpp>
#include "Synchronize.h"
#include "LuceneThread.h"

namespace Lucene {

Synchronize::Synchronize() {
    lockThread = 0;
    recursionCount = 0;
}

Synchronize::~Synchronize() {
}

void Synchronize::createSync(SynchronizePtr& sync) {
    static boost::mutex lockMutex;
    boost::mutex::scoped_lock syncLock(lockMutex);
    if (!sync) {
        sync = newInstance<Synchronize>();
    }
}

void Synchronize::lock(int32_t timeout) {
    if (timeout > 0) {
        mutexSynchronize.timed_lock(boost::posix_time::milliseconds(timeout));
    } else {
        mutexSynchronize.lock();
    }
    lockThread = LuceneThread::currentId();
    ++recursionCount;
}

void Synchronize::unlock() {
    if (--recursionCount == 0) {
        lockThread = 0;
    }
    mutexSynchronize.unlock();
}

int32_t Synchronize::unlockAll() {
    int32_t count = recursionCount;
    for (int32_t unlock = 0; unlock < count; ++unlock) {
        this->unlock();
    }
    return count;
}

bool Synchronize::holdsLock() {
    return (lockThread == LuceneThread::currentId() && recursionCount > 0);
}

SyncLock::SyncLock(const SynchronizePtr& sync, int32_t timeout) {
    this->sync = sync;
    lock(timeout);
}

SyncLock::~SyncLock() {
    if (sync) {
        sync->unlock();
    }
}

void SyncLock::lock(int32_t timeout) {
    if (sync) {
        sync->lock(timeout);
    }
}

}
