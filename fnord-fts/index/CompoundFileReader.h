/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COMPOUNDFILEREADER_H
#define COMPOUNDFILEREADER_H

#include "fnord-fts/store/Directory.h"
#include "fnord-fts/store/BufferedIndexInput.h"

namespace Lucene {

/// Class for accessing a compound stream.
/// This class implements a directory, but is limited to only read operations.
/// Directory methods that would normally modify data throw an exception.
class CompoundFileReader : public Directory {
public:
    CompoundFileReader(const DirectoryPtr& dir, const String& name);
    CompoundFileReader(const DirectoryPtr& dir, const String& name, int32_t readBufferSize);
    virtual ~CompoundFileReader();

    LUCENE_CLASS(CompoundFileReader);

protected:
    struct FileEntry {
        FileEntry(int64_t offset = 0, int64_t length = 0) {
            this->offset = offset;
            this->length = length;
        }
        int64_t offset;
        int64_t length;
    };
    typedef std::shared_ptr<FileEntry> FileEntryPtr;
    typedef HashMap<String, FileEntryPtr> MapStringFileEntryPtr;

    DirectoryPtr directory;
    String fileName;
    int32_t readBufferSize;
    IndexInputPtr stream;
    MapStringFileEntryPtr entries;

protected:
    void ConstructReader(const DirectoryPtr& dir, const String& name, int32_t readBufferSize);

public:
    DirectoryPtr getDirectory();
    String getName();
    virtual void close();
    virtual IndexInputPtr openInput(const String& name);
    virtual IndexInputPtr openInput(const String& name, int32_t bufferSize);

    /// Returns an array of strings, one for each file in the directory.
    virtual HashSet<String> listAll();

    /// Returns true if a file with the given name exists.
    virtual bool fileExists(const String& name);

    /// Returns the time the compound file was last modified.
    virtual uint64_t fileModified(const String& name);

    /// Set the modified time of the compound file to now.
    virtual void touchFile(const String& name);

    /// Not implemented
    virtual void deleteFile(const String& name);

    /// Not implemented
    virtual void renameFile(const String& from, const String& to);

    /// Returns the length of a file in the directory.
    virtual int64_t fileLength(const String& name);

    /// Not implemented
    virtual IndexOutputPtr createOutput(const String& name);

    /// Not implemented
    virtual LockPtr makeLock(const String& name);
};

/// Implementation of an IndexInput that reads from a portion of the compound file.
class CSIndexInput : public BufferedIndexInput {
public:
    CSIndexInput();
    CSIndexInput(const IndexInputPtr& base, int64_t fileOffset, int64_t length);
    CSIndexInput(const IndexInputPtr& base, int64_t fileOffset, int64_t length, int32_t readBufferSize);
    virtual ~CSIndexInput();

    LUCENE_CLASS(CSIndexInput);

public:
    IndexInputPtr base;
    int64_t fileOffset;
    int64_t _length;

public:
    /// Closes the stream to further operations.
    virtual void close();

    virtual int64_t length();

    /// Returns a clone of this stream.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

protected:
    /// Implements buffer refill.  Reads bytes from the current position in the input.
    /// @param b the array to read bytes into
    /// @param offset the offset in the array to start storing bytes
    /// @param len the number of bytes to read
    virtual void readInternal(uint8_t* b, int32_t offset, int32_t length);

    /// Implements seek.  Sets current position in this file, where the next {@link
    /// #readInternal(byte[],int,int)} will occur.
    virtual void seekInternal(int64_t pos);
};

}

#endif
