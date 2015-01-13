/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "LuceneObject.h"

namespace Lucene {

/// A Directory is a flat list of files.  Files may be written once, when they are created.  Once a file
/// is created it may only be opened for read, or deleted.  Random access is permitted both when reading
/// and writing.  Directory locking is implemented by an instance of {@link LockFactory}, and can be changed
/// for each Directory instance using {@link #setLockFactory}.
class Directory : public LuceneObject {
public:
    Directory();
    virtual ~Directory();

    LUCENE_CLASS(Directory);

protected:
    bool isOpen;

    /// Holds the LockFactory instance (implements locking for this Directory instance).
    LockFactoryPtr lockFactory;

public:
    /// Returns an array of strings, one for each file in the directory.
    virtual HashSet<String> listAll() = 0;

    /// Returns true if a file with the given name exists.
    virtual bool fileExists(const String& name) = 0;

    /// Returns the time the named file was last modified.
    virtual uint64_t fileModified(const String& name) = 0;

    /// Set the modified time of an existing file to now.
    virtual void touchFile(const String& name) = 0;

    /// Removes an existing file in the directory.
    virtual void deleteFile(const String& name) = 0;

    /// Returns the length of a file in the directory.
    virtual int64_t fileLength(const String& name) = 0;

    /// Creates a new, empty file in the directory with the given name.
    /// Returns a stream writing this file.
    virtual IndexOutputPtr createOutput(const String& name) = 0;

    /// Returns a stream reading an existing file.
    virtual IndexInputPtr openInput(const String& name) = 0;

    /// Closes the store.
    virtual void close() = 0;

    /// Ensure that any writes to this file are moved to stable storage.  Lucene uses this to properly commit
    /// changes to the index, to prevent a machine/OS crash from corrupting the index.
    virtual void sync(const String& name);

    /// Returns a stream reading an existing file, with the specified read buffer size.  The particular Directory
    /// implementation may ignore the buffer size.  Currently the only Directory implementations that respect
    /// this parameter are {@link FSDirectory} and {@link CompoundFileReader}.
    virtual IndexInputPtr openInput(const String& name, int32_t bufferSize);

    /// Construct a {@link Lock}.
    /// @param name the name of the lock file.
    virtual LockPtr makeLock(const String& name);

    /// Attempt to clear (forcefully unlock and remove) the specified lock.  Only call this at a time when you
    /// are certain this lock is no longer in use.
    /// @param name name of the lock to be cleared.
    void clearLock(const String& name);

    /// Set the LockFactory that this Directory instance should use for its locking implementation.  Each * instance
    /// of LockFactory should only be used for one directory (ie, do not share a single instance across multiple
    /// Directories).
    /// @param lockFactory instance of {@link LockFactory}.
    void setLockFactory(const LockFactoryPtr& lockFactory);

    /// Get the LockFactory that this Directory instance is using for its locking implementation.  Note that this
    /// may be null for Directory implementations that provide their own locking implementation.
    LockFactoryPtr getLockFactory();

    /// Return a string identifier that uniquely differentiates this Directory instance from other Directory
    /// instances. This ID should be the same if two Directory instances are considered "the same index".
    /// This is how locking "scopes" to the right index.
    virtual String getLockID();

    virtual String toString();

    /// Copy contents of a directory src to a directory dest. If a file in src already exists in dest then the one
    /// in dest will be blindly overwritten.  NOTE: the source directory cannot change while this method is running.
    /// Otherwise the results are undefined.
    /// @param src source directory.
    /// @param dest destination directory.
    /// @param closeDirSrc if true, call {@link #close()} method on source directory.
    static void copy(const DirectoryPtr& src, const DirectoryPtr& dest, bool closeDirSrc);

protected:
    /// @throws AlreadyClosed if this Directory is closed.
    void ensureOpen();
};

}

#endif
