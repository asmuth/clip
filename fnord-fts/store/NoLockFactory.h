/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NOLOCKFACTORY_H
#define NOLOCKFACTORY_H

#include "fnord-fts/store/LockFactory.h"

namespace fnord {
namespace fts {

/// Use this {@link LockFactory} to disable locking entirely.  Only one instance of this lock is created.
/// You should call {@link #getNoLockFactory()} to get the instance.
///
/// @see LockFactory
class NoLockFactory : public LockFactory {
public:
    virtual ~NoLockFactory();

    LUCENE_CLASS(NoLockFactory);

private:
    static NoLockPtr getSingletonLock();

public:
    static NoLockFactoryPtr getNoLockFactory();

    /// Return a new Lock instance identified by lockName.
    virtual LockPtr makeLock(const String& lockName);

    /// Attempt to clear (forcefully unlock and remove) the specified lock.  Only call this at a time when you
    /// are certain this lock is no longer in use.
    virtual void clearLock(const String& lockName);
};

}

}
#endif
