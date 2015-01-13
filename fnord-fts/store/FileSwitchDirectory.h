/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILESWITCHDIRECTORY_H
#define FILESWITCHDIRECTORY_H

#include "fnord-fts/store/Directory.h"

namespace Lucene {

/// A Directory instance that switches files between two other
/// Directory instances.
///
/// Files with the specified extensions are placed in the primary
/// directory; others are placed in the secondary directory.  The
/// provided Set must not change once passed to this class, and
/// must allow multiple threads to call contains at once.
class FileSwitchDirectory : public Directory {
public:
    FileSwitchDirectory(HashSet<String> primaryExtensions, const DirectoryPtr& primaryDir, const DirectoryPtr& secondaryDir, bool doClose);
    virtual ~FileSwitchDirectory();

    LUCENE_CLASS(FileSwitchDirectory);

protected:
    HashSet<String> primaryExtensions;
    DirectoryPtr primaryDir;
    DirectoryPtr secondaryDir;
    bool doClose;

public:
    /// Return the primary directory.
    DirectoryPtr getPrimaryDir();

    /// Return the secondary directory.
    DirectoryPtr getSecondaryDir();

    /// Closes the store.
    virtual void close();

    /// Returns an array of strings, one for each file in the directory.
    virtual HashSet<String> listAll();

    /// Utility method to return a file's extension.
    static String getExtension(const String& name);

    /// Returns true if a file with the given name exists.
    virtual bool fileExists(const String& name);

    /// Returns the time the named file was last modified.
    virtual uint64_t fileModified(const String& name);

    /// Set the modified time of an existing file to now.
    virtual void touchFile(const String& name);

    /// Removes an existing file in the directory.
    virtual void deleteFile(const String& name);

    /// Returns the length of a file in the directory.
    virtual int64_t fileLength(const String& name);

    /// Creates a new, empty file in the directory with the given name.
    /// Returns a stream writing this file.
    virtual IndexOutputPtr createOutput(const String& name);

    /// Ensure that any writes to this file are moved to stable storage.
    /// Lucene uses this to properly commit changes to the index, to
    /// prevent a machine/OS crash from corrupting the index.
    virtual void sync(const String& name);

    /// Returns a stream reading an existing file, with the specified
    /// read buffer size.  The particular Directory implementation may
    /// ignore the buffer size.
    virtual IndexInputPtr openInput(const String& name);

protected:
    DirectoryPtr getDirectory(const String& name);
};

}

#endif
