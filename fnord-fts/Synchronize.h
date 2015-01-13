/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

#include <boost/thread/recursive_mutex.hpp>
#include "Lucene.h"

namespace Lucene {

/// Utility class to support locking via a mutex.
class Synchronize {
public:
    Synchronize();
    virtual ~Synchronize();

protected:
    boost::recursive_timed_mutex mutexSynchronize;
    int64_t lockThread;
    int32_t recursionCount;

public:
    /// create a new Synchronize instance atomically.
    static void createSync(SynchronizePtr& sync);

    /// Lock mutex using an optional timeout.
    void lock(int32_t timeout = 0);

    /// Unlock mutex.
    void unlock();

    /// Unlock all recursive mutex.
    int32_t unlockAll();

    /// Returns true if mutex is currently locked by current thread.
    bool holdsLock();
};

/// Utility class to support scope locking.
class SyncLock {
public:
    SyncLock(const SynchronizePtr& sync, int32_t timeout = 0);

    template <class OBJECT>
    SyncLock(OBJECT object, int32_t timeout = 0) {
        this->sync = object->getSync();
        lock(timeout);
    }

    virtual ~SyncLock();

protected:
    SynchronizePtr sync;

protected:
    void lock(int32_t timeout);
};

}

#endif
