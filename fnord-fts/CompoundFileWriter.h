/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COMPOUNDFILEWRITER_H
#define COMPOUNDFILEWRITER_H

#include "LuceneObject.h"

namespace Lucene {

/// Combines multiple files into a single compound file.
/// The file format:
///    VInt fileCount
///    {Directory}
///    fileCount entries with the following structure:
///        int64_t dataOffset
///        String fileName
///    {File Data}
///    fileCount entries with the raw data of the corresponding file
///
/// The fileCount integer indicates how many files are contained in this compound file. The {directory}
/// that follows has that many entries. Each directory entry contains a long pointer to the start of
/// this file's data section, and a string with that file's name.
class CompoundFileWriter : public LuceneObject {
public:
    CompoundFileWriter(const DirectoryPtr& dir, const String& name, const CheckAbortPtr& checkAbort = CheckAbortPtr());
    virtual ~CompoundFileWriter();

    LUCENE_CLASS(CompoundFileWriter);

protected:
    struct FileEntry {
        /// source file
        String file;

        /// temporary holder for the start of directory entry for this file
        int64_t directoryOffset;

        /// temporary holder for the start of this file's data section
        int64_t dataOffset;
    };

    DirectoryWeakPtr _directory;
    String fileName;
    HashSet<String> ids;
    Collection<FileEntry> entries;
    bool merged;
    CheckAbortPtr checkAbort;

public:
    /// Returns the directory of the compound file.
    DirectoryPtr getDirectory();

    /// Returns the name of the compound file.
    String getName();

    /// Add a source stream. file is the string by which the sub-stream will be known in the
    /// compound stream.
    void addFile(const String& file);

    /// Merge files with the extensions added up to now.  All files with these extensions are
    /// combined sequentially into the compound stream. After successful merge, the source
    /// are deleted.files
    void close();

protected:
    /// Copy the contents of the file with specified extension into the provided output stream.
    /// Use the provided buffer for moving data to reduce memory allocation.
    void copyFile(const FileEntry& source, const IndexOutputPtr& os, ByteArray buffer);
};

}

#endif
