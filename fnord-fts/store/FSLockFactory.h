/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FSLOCKFACTORY_H
#define FSLOCKFACTORY_H

#include "fnord-fts/store/LockFactory.h"

namespace fnord {
namespace fts {

/// Base class for file system based locking implementation.
class FSLockFactory : public LockFactory {
protected:
    FSLockFactory();

public:
    virtual ~FSLockFactory();

    LUCENE_CLASS(FSLockFactory);

protected:
    /// Directory for the lock files.
    String lockDir;

public:
    /// Set the lock directory. This method can be only called once to
    /// initialize the lock directory. It is used by {@link FSDirectory}
    /// to set the lock directory to itself.  Subclasses can also use
    /// this method to set the directory in the constructor.
    void setLockDir(const String& lockDir);

    /// Retrieve the lock directory.
    String getLockDir();
};

}

}
#endif
