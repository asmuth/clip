/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOCK_H
#define LOCK_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// An interprocess mutex lock.
/// @see Directory#makeLock(const String&)
class Lock : public LuceneObject {
public:
    virtual ~Lock();
    LUCENE_CLASS(Lock);

public:
    /// How long {@link #obtain(int64_t)} waits, in milliseconds, in between attempts to acquire the lock.
    static const int32_t LOCK_OBTAIN_WAIT_FOREVER;

    /// Pass this value to {@link #obtain(int64_t)} to try forever to obtain the lock.
    static const int32_t LOCK_POLL_INTERVAL;

public:
    /// Attempts to obtain exclusive access and immediately return upon success or failure.
    /// @return true if exclusive access is obtained.
    virtual bool obtain() = 0;

    /// Releases exclusive access.
    virtual void release() = 0;

    /// Returns true if the resource is currently locked. Note that one must still call {@link #obtain()}
    /// before using the resource.
    virtual bool isLocked() = 0;

    /// Attempts to obtain an exclusive lock within amount of time given. Polls once per {@link #LOCK_POLL_INTERVAL}
    /// (currently 1000) milliseconds until lockWaitTimeout is passed.
    /// @param lockWaitTimeout length of time to wait in milliseconds or {@link #LOCK_OBTAIN_WAIT_FOREVER}
    /// to retry forever.
    /// @return true if lock was obtained.
    bool obtain(int32_t lockWaitTimeout);
};

}

}
#endif
