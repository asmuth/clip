/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NATIVEFSLOCKFACTORY_H
#define NATIVEFSLOCKFACTORY_H

#include "FSLockFactory.h"

namespace Lucene {

/// Implements {@link LockFactory} using native file lock.
/// @see LockFactory
class NativeFSLockFactory : public FSLockFactory {
public:
    /// Create a NativeFSLockFactory instance, storing lock files into
    /// the specified lockDirName.
    /// @param lockDirName where lock files are created.
    NativeFSLockFactory(const String& lockDirName = EmptyString);
    virtual ~NativeFSLockFactory();

    LUCENE_CLASS(NativeFSLockFactory);

public:
    /// Return a new Lock instance identified by lockName.
    /// @param lockName name of the lock to be created.
    virtual LockPtr makeLock(const String& lockName);

    /// Attempt to clear (forcefully unlock and remove) the
    /// specified lock.  Only call this at a time when you are
    /// certain this lock is no longer in use.
    /// @param lockName name of the lock to be cleared.
    virtual void clearLock(const String& lockName);

protected:
    /// Simple test to verify locking system is "working".  On NFS, if
    /// it's mis-configured, you can hit long (35 second) timeouts which
    /// cause Lock.obtain to take far too long (it assumes the obtain()
    /// call takes zero time).
    void acquireTestLock();
};

}

#endif
