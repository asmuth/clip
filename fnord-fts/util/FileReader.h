/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILEREADER_H
#define FILEREADER_H

#include "fnord-fts/util/Reader.h"

namespace Lucene {

/// Convenience class for reading character files.
class FileReader : public Reader {
public:
    /// Creates a new FileReader, given the file name to read from.
    FileReader(const String& fileName);
    virtual ~FileReader();

    LUCENE_CLASS(FileReader);

protected:
    ifstreamPtr file;
    int64_t _length;
    ByteArray fileBuffer;

public:
    static const int32_t FILE_EOF;
    static const int32_t FILE_ERROR;

public:
    /// Read a single character.
    virtual int32_t read();

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);

    /// Close the stream.
    virtual void close();

    /// Tell whether this stream supports the mark() operation
    virtual bool markSupported();

    /// Reset the stream.
    virtual void reset();

    /// The number of bytes in the file.
    virtual int64_t length();
};

}

#endif
