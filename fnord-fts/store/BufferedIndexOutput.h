/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BUFFEREDINDEXOUTPUT_H
#define BUFFEREDINDEXOUTPUT_H

#include "fnord-fts/store/IndexOutput.h"

namespace Lucene {

/// Base implementation class for buffered {@link IndexOutput}.
class BufferedIndexOutput : public IndexOutput {
public:
    BufferedIndexOutput();
    virtual ~BufferedIndexOutput();

    LUCENE_CLASS(BufferedIndexOutput);

public:
    static const int32_t BUFFER_SIZE;

protected:
    int64_t bufferStart; // position in file of buffer
    int32_t bufferPosition; // position in buffer
    ByteArray buffer;

public:
    /// Writes a single byte.
    /// @see IndexInput#readByte()
    virtual void writeByte(uint8_t b);

    /// Writes an array of bytes.
    /// @param b the bytes to write.
    /// @param length the number of bytes to write.
    /// @see IndexInput#readBytes(uint8_t*, int32_t, int32_t)
    virtual void writeBytes(const uint8_t* b, int32_t offset, int32_t length);

    /// Forces any buffered output to be written.
    virtual void flush();

    /// Implements buffer write.  Writes bytes at the current
    /// position in the output.
    /// @param b the bytes to write.
    /// @param offset the offset in the byte array.
    /// @param length the number of bytes to write.
    virtual void flushBuffer(const uint8_t* b, int32_t offset, int32_t length);

    /// Closes this stream to further operations.
    virtual void close();

    /// Returns the current position in this file, where the next write will occur.
    /// @see #seek(long)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next write will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// The number of bytes in the file.
    virtual int64_t length() = 0;

protected:
    /// Implements buffer write.  Writes bytes at the current
    /// position in the output.
    /// @param b the bytes to write.
    /// @param length the number of bytes to write.
    void flushBuffer(const uint8_t* b, int32_t length);
};

}

#endif
