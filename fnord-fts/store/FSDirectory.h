/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FSDIRECTORY_H
#define FSDIRECTORY_H

#include "fnord-fts/store/Directory.h"

namespace Lucene {

/// Base class for Directory implementations that store index files in the file system.  There are currently three
/// core subclasses:
///
/// {@link SimpleFSDirectory} is a straightforward implementation using std::ofstream and std::ifstream.
///
/// {@link MMapDirectory} uses memory-mapped IO when reading. This is a good choice if you have plenty of virtual
/// memory relative to your index size, eg if you are running on a 64 bit operating system, oryour index sizes are
/// small enough to fit into the virtual memory space.
///
/// For users who have no reason to prefer a specific implementation, it's best to simply use {@link #open}.  For
/// all others, you should instantiate the desired implementation directly.
///
/// The locking implementation is by default {@link NativeFSLockFactory}, but can be changed by passing in a custom
/// {@link LockFactory} instance.
/// @see Directory
class FSDirectory : public Directory {
protected:
    /// Create a new FSDirectory for the named location (ctor for subclasses).
    /// @param path the path of the directory.
    /// @param lockFactory the lock factory to use, or null for the default ({@link NativeFSLockFactory})
    FSDirectory(const String& path, const LockFactoryPtr& lockFactory);

public:
    virtual ~FSDirectory();

    LUCENE_CLASS(FSDirectory);

public:
    /// Default read chunk size.  This is a conditional default based on operating system.
    /// @see #setReadChunkSize
    static const int32_t DEFAULT_READ_CHUNK_SIZE;

protected:
    bool checked;

    /// The underlying filesystem directory.
    String directory;

    /// @see #DEFAULT_READ_CHUNK_SIZE
    int32_t chunkSize;

public:
    /// Creates an FSDirectory instance.
    static FSDirectoryPtr open(const String& path);

    /// Just like {@link #open(File)}, but allows you to also specify a custom {@link LockFactory}.
    static FSDirectoryPtr open(const String& path, const LockFactoryPtr& lockFactory);

    /// Lists all files (not subdirectories) in the directory.
    /// @throws NoSuchDirectoryException if the directory does not exist, or does exist but is not a directory.
    static HashSet<String> listAll(const String& dir);

    /// Returns the time the named file was last modified.
    static uint64_t fileModified(const String& directory, const String& name);

    /// Create file system directory.
    void createDir();

    /// Return file system directory.
    String getFile();

    /// Sets the maximum number of bytes read at once from the underlying file during {@link IndexInput#readBytes}.
    /// The default value is {@link #DEFAULT_READ_CHUNK_SIZE}.  Changes to this value will not impact any already-opened
    /// {@link IndexInput}s.  You should call this before attempting to open an index on the directory.  This value should
    /// be as large as possible to reduce any possible performance impact.
    void setReadChunkSize(int32_t chunkSize);

    /// The maximum number of bytes to read at once from the underlying file during {@link IndexInput#readBytes}.
    /// @see #setReadChunkSize
    int32_t getReadChunkSize();

    /// Lists all files (not subdirectories) in the directory.
    /// @see #listAll(const String&)
    virtual HashSet<String> listAll();

    /// Returns true if a file with the given name exists.
    virtual bool fileExists(const String& name);

    /// Returns the time the named file was last modified.
    virtual uint64_t fileModified(const String& name);

    /// Set the modified time of an existing file to now.
    virtual void touchFile(const String& name);

    /// Removes an existing file in the directory.
    virtual void deleteFile(const String& name);

    /// Returns the length in bytes of a file in the directory.
    virtual int64_t fileLength(const String& name);

    /// Ensure that any writes to this file are moved to stable storage.  Lucene uses this to properly commit changes to
    /// the index, to prevent a machine/OS crash from corrupting the index.
    virtual void sync(const String& name);

    /// Returns a stream reading an existing file, with the specified read buffer size.  The particular Directory
    /// implementation may ignore the buffer size.
    virtual IndexInputPtr openInput(const String& name);

    /// Returns a stream reading an existing file, with the specified read buffer size.  The particular Directory
    /// implementation may ignore the buffer size.  Currently the only Directory implementations that respect this parameter
    /// are {@link FSDirectory} and {@link CompoundFileReader}.
    virtual IndexInputPtr openInput(const String& name, int32_t bufferSize);

    /// Return a string identifier that uniquely differentiates this Directory instance from other Directory instances.
    virtual String getLockID();

    /// Closes the store to future operations.
    virtual void close();

    /// For debug output.
    virtual String toString();

protected:
    /// Initializes the directory to create a new file with the given name. This method should be used in {@link #createOutput}.
    void initOutput(const String& name);
};

}

#endif
