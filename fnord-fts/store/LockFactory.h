/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOCKFACTORY_H
#define LOCKFACTORY_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Base class for Locking implementation.  {@link Directory} uses
/// instances of this class to implement locking.
/// Note that there are some useful tools to verify that
/// your LockFactory is working correctly: {@link
/// VerifyingLockFactory}, {@link LockStressTest}, {@link
/// LockVerifyServer}.
/// @see LockVerifyServer
/// @see LockStressTest
/// @see VerifyingLockFactory
class LockFactory : public LuceneObject {
public:
    virtual ~LockFactory();

    LUCENE_CLASS(LockFactory);

protected:
    String lockPrefix;

public:
    /// Set the prefix in use for all locks created in this LockFactory.  This is normally called once, when a
    /// Directory gets this LockFactory instance.  However, you can also call this (after this instance is
    /// assigned to a Directory) to override the prefix in use.  This is helpful if you're running Lucene on
    /// machines that have different mount points for the same shared directory.
    virtual void setLockPrefix(const String& lockPrefix);

    /// Get the prefix in use for all locks created in this LockFactory.
    virtual String getLockPrefix();

    /// Return a new Lock instance identified by lockName.
    /// @param lockName name of the lock to be created.
    virtual LockPtr makeLock(const String& lockName) = 0;

    /// Attempt to clear (forcefully unlock and remove) the specified lock.  Only call this at a time when you
    /// are certain this lock is no longer in use.
    /// @param lockName name of the lock to be cleared.
    virtual void clearLock(const String& lockName) = 0;
};

}

}
#endif
