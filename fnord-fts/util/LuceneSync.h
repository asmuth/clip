/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENESYNC_H
#define LUCENESYNC_H

#include "fnord-fts/fts.h"

namespace Lucene {

/// Base class for all Lucene synchronised classes
class LuceneSync {
public:
    virtual ~LuceneSync();

protected:
    SynchronizePtr objectLock;
    LuceneSignalPtr objectSignal;

public:
    /// Return this object synchronize lock.
    virtual SynchronizePtr getSync();

    /// Return this object signal.
    virtual LuceneSignalPtr getSignal();

    /// Lock this object using an optional timeout.
    virtual void lock(int32_t timeout = 0);

    /// Unlock this object.
    virtual void unlock();

    /// Returns true if this object is currently locked by current thread.
    virtual bool holdsLock();

    /// Wait for signal using an optional timeout.
    virtual void wait(int32_t timeout = 0);

    /// Notify all threads waiting for signal.
    virtual void notifyAll();
};

}

#endif
