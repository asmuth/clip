/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SINGLEINSTANCELOCKFACTORY_H
#define _SINGLEINSTANCELOCKFACTORY_H

#include "Lock.h"

namespace Lucene {

class SingleInstanceLock : public Lock {
public:
    SingleInstanceLock(HashSet<String> locks, const String& lockName);
    virtual ~SingleInstanceLock();

    LUCENE_CLASS(SingleInstanceLock);

protected:
    HashSet<String> locks;
    String lockName;

public:
    /// Attempts to obtain exclusive access and immediately return
    /// upon success or failure.
    /// @return true if exclusive access is obtained.
    virtual bool obtain();

    /// Releases exclusive access.
    virtual void release();

    /// Returns true if the resource is currently locked. Note that
    /// one must still call {@link #obtain()} before using the resource.
    virtual bool isLocked();

    /// Returns derived object name.
    virtual String toString();
};

}

#endif
