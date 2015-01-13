/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLEFSDIRECTORY_H
#define SIMPLEFSDIRECTORY_H

#include "FSDirectory.h"

namespace Lucene {

/// A straightforward implementation of {@link FSDirectory} using std::ofstream and std::ifstream.
class SimpleFSDirectory : public FSDirectory {
public:
    /// Create a new SimpleFSDirectory for the named location  and {@link NativeFSLockFactory}.
    /// @param path the path of the directory.
    /// @param lockFactory the lock factory to use, or null for the default ({@link NativeFSLockFactory})
    SimpleFSDirectory(const String& path, const LockFactoryPtr& lockFactory = LockFactoryPtr());
    virtual ~SimpleFSDirectory();

    LUCENE_CLASS(SimpleFSDirectory);

public:
    /// Creates an IndexOutput for the file with the given name.
    virtual IndexOutputPtr createOutput(const String& name);

    /// Returns a stream reading an existing file, with the specified read buffer size.  The particular Directory implementation may ignore the buffer size.
    virtual IndexInputPtr openInput(const String& name);

    /// Creates an IndexInput for the file with the given name.
    virtual IndexInputPtr openInput(const String& name, int32_t bufferSize);
};

}

#endif
