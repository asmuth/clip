/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#include "fnord-fts/util/Reader.h"

namespace fnord {
namespace fts {

/// Read text from a character-input stream, buffering characters so as to provide
/// for the efficient reading of characters, arrays, and lines.
class BufferedReader : public Reader {
public:
    /// Create a buffering character-input stream.
    BufferedReader(const ReaderPtr& reader, int32_t size = READER_BUFFER);
    virtual ~BufferedReader();

    LUCENE_CLASS(BufferedReader);

protected:
    ReaderPtr reader;
    int32_t bufferSize;
    int32_t bufferLength; // end of valid bytes
    int32_t bufferPosition; // next byte to read
    CharArray buffer;

public:
    static const int32_t READER_BUFFER;

public:
    /// Read a single character.
    virtual int32_t read();

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* b, int32_t offset, int32_t length);

    /// Read a line of text.
    virtual bool readLine(String& line);

    /// Close the stream.
    virtual void close();

    /// Tell whether this stream supports the mark() operation
    virtual bool markSupported();

    /// Reset the stream.
    virtual void reset();

protected:
    /// Refill buffer in preparation for reading.
    int32_t refill();

    /// Read a single character without moving position.
    int32_t peek();
};

}

}
#endif
