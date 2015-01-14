/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RAMDIRECTORY_H
#define RAMDIRECTORY_H

#include "fnord-fts/store/Directory.h"

namespace fnord {
namespace fts {

/// A memory-resident {@link Directory} implementation.  Locking implementation is by default the
/// {@link SingleInstanceLockFactory} but can be changed with {@link #setLockFactory}.
/// Lock acquisition sequence:  RAMDirectory, then RAMFile
class RAMDirectory : public Directory {
public:
    /// Constructs an empty {@link Directory}.
    RAMDirectory();

    /// Creates a new RAMDirectory instance from a different Directory implementation.
    /// This can be used to load a disk-based index into memory.
    ///
    /// This should be used only with indices that can fit into memory.
    ///
    /// Note that the resulting RAMDirectory instance is fully independent from the
    /// original Directory (it is a complete copy).  Any subsequent changes to the
    /// original Directory will not be visible in the RAMDirectory instance.
    /// @param dir a Directory value
    RAMDirectory(const DirectoryPtr& dir);

    RAMDirectory(const DirectoryPtr& dir, bool closeDir);

    virtual ~RAMDirectory();

    LUCENE_CLASS(RAMDirectory);

INTERNAL:
    int64_t _sizeInBytes;
    MapStringRAMFile fileMap;

protected:
    DirectoryWeakPtr _dirSource;
    bool copyDirectory;
    bool closeDir;

public:
    virtual void initialize();

    /// Returns an array of strings, one for each file in the directory.
    virtual HashSet<String> listAll();

    /// Returns true if a file with the given name exists.
    virtual bool fileExists(const String& name);

    /// Returns the time the named file was last modified.
    virtual uint64_t fileModified(const String& name);

    /// Set the modified time of an existing file to now.
    virtual void touchFile(const String& name);

    /// Returns the length of a file in the directory.
    virtual int64_t fileLength(const String& name);

    /// Return total size in bytes of all files in this directory.
    /// This is currently quantized to RAMOutputStream::BUFFER_SIZE.
    int64_t sizeInBytes();

    /// Removes an existing file in the directory.
    virtual void deleteFile(const String& name);

    /// Creates a new, empty file in the directory with the given name.
    /// Returns a stream writing this file.
    virtual IndexOutputPtr createOutput(const String& name);

    /// Returns a stream reading an existing file.
    virtual IndexInputPtr openInput(const String& name);

    /// Closes the store.
    virtual void close();
};

}

}
#endif
