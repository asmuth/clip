/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SINGLEINSTANCELOCKFACTORY_H
#define SINGLEINSTANCELOCKFACTORY_H

#include "fnord-fts/store/LockFactory.h"

namespace Lucene {

/// Implements {@link LockFactory} for a single in-process instance, meaning all
/// locking will take place through this one instance.  Only use this {@link LockFactory}
/// when you are certain all IndexReaders and IndexWriters for a given index are running
/// against a single shared in-process Directory instance.  This is currently the
/// default locking for RAMDirectory.
/// @see LockFactory
class SingleInstanceLockFactory : public LockFactory {
public:
    SingleInstanceLockFactory();
    virtual ~SingleInstanceLockFactory();

    LUCENE_CLASS(SingleInstanceLockFactory);

protected:
    HashSet<String> locks;

public:
    /// Return a new Lock instance identified by lockName.
    /// @param lockName name of the lock to be created.
    virtual LockPtr makeLock(const String& lockName);

    /// Attempt to clear (forcefully unlock and remove) the
    /// specified lock.  Only call this at a time when you are
    /// certain this lock is no longer in use.
    /// @param lockName name of the lock to be cleared.
    virtual void clearLock(const String& lockName);
};

}

#endif
