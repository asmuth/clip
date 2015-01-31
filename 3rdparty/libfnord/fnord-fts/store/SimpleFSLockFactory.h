/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLEFSLOCKFACTORY_H
#define SIMPLEFSLOCKFACTORY_H

#include "fnord-fts/store/FSLockFactory.h"
#include "fnord-fts/store/Lock.h"

namespace fnord {
namespace fts {

/// Implements {@link LockFactory} using {@link File#createNewFile()}.
/// @see LockFactory
class SimpleFSLockFactory : public FSLockFactory {
public:
    /// Create a SimpleFSLockFactory instance, with null (unset) lock directory. When you pass this factory
    /// to a {@link FSDirectory} subclass, the lock directory is automatically set to the directory itself.
    /// Be sure to create one instance for each directory your create!
    SimpleFSLockFactory();

    /// Instantiate using the provided directory name.
    /// @param lockDir where lock files should be created.
    SimpleFSLockFactory(const String& lockDir);

    virtual ~SimpleFSLockFactory();

    LUCENE_CLASS(SimpleFSLockFactory);

public:
    /// Return a new Lock instance identified by lockName.
    virtual LockPtr makeLock(const String& lockName);

    /// Attempt to clear (forcefully unlock and remove) the specified lock.
    virtual void clearLock(const String& lockName);
};

}

}
#endif
