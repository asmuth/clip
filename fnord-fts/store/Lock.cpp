/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Lock.h"
#include "LuceneThread.h"

namespace Lucene {

/// How long {@link #obtain(int64_t)} waits, in milliseconds, in between attempts to acquire the lock.
const int32_t Lock::LOCK_OBTAIN_WAIT_FOREVER = -1;

/// Pass this value to {@link #obtain(int64_t)} to try forever to obtain the lock.
const int32_t Lock::LOCK_POLL_INTERVAL = 1000;

Lock::~Lock() {
}

bool Lock::obtain(int32_t lockWaitTimeout) {
    bool locked = obtain();
    int32_t maxSleepCount = lockWaitTimeout / LOCK_POLL_INTERVAL;
    int32_t sleepCount = 0;
    while (!locked) {
        if (lockWaitTimeout != LOCK_OBTAIN_WAIT_FOREVER && sleepCount++ >= maxSleepCount) {
            boost::throw_exception(LockObtainFailedException(L"Lock obtain timed out"));
        }
        LuceneThread::threadSleep(LOCK_POLL_INTERVAL);
        locked = obtain();
    }
    return locked;
}

}
