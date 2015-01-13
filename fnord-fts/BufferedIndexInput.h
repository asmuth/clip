/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BUFFEREDINDEXINPUT_H
#define BUFFEREDINDEXINPUT_H

#include "IndexInput.h"

namespace Lucene {

/// Base implementation class for buffered {@link IndexInput}.
class BufferedIndexInput : public IndexInput {
public:
    /// Construct BufferedIndexInput with a specific bufferSize.
    BufferedIndexInput(int32_t bufferSize = BUFFER_SIZE);
    virtual ~BufferedIndexInput();

    LUCENE_CLASS(BufferedIndexInput);

public:
    /// Default buffer size.
    static const int32_t BUFFER_SIZE;

protected:
    int32_t bufferSize;
    int64_t bufferStart; // position in file of buffer
    int32_t bufferLength; // end of valid bytes
    int32_t bufferPosition; // next byte to read
    ByteArray buffer;

public:
    /// Reads and returns a single byte.
    /// @see IndexOutput#writeByte(uint8_t)
    virtual uint8_t readByte();

    /// Change the buffer size used by this IndexInput.
    void setBufferSize(int32_t newSize);

    /// Returns buffer size.
    /// @see #setBufferSize
    int32_t getBufferSize();

    /// Reads a specified number of bytes into an array at the specified offset.
    /// @param b the array to read bytes into.
    /// @param offset the offset in the array to start storing bytes.
    /// @param length the number of bytes to read.
    /// @see IndexOutput#writeBytes(const uint8_t*,int)
    /// @see #readInternal(uint8_t*, int32_t, int32_t)
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length);

    /// Reads a specified number of bytes into an array at the specified offset with control over whether the
    /// read should be buffered (callers who have their own buffer should pass in "false" for useBuffer).
    /// Currently only {@link BufferedIndexInput} respects this parameter.
    /// @param b the array to read bytes into.
    /// @param offset the offset in the array to start storing bytes.
    /// @param length the number of bytes to read.
    /// @param useBuffer set to false if the caller will handle buffering.
    /// @see IndexOutput#writeBytes(const uint8_t*,int)
    /// @see #readInternal(uint8_t*, int32_t, int32_t)
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length, bool useBuffer);

    /// Closes the stream to further operations.
    virtual void close();

    /// Returns the current position in this file, where the next read will occur.
    /// @see #seek(int64_t)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next read will occur.
    /// @see #getFilePointer()
    /// @see #seekInternal(int64_t)
    virtual void seek(int64_t pos);

    /// Returns a clone of this stream.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

protected:
    virtual void newBuffer(ByteArray newBuffer);

    void checkBufferSize(int32_t bufferSize);

    /// Refill buffer in preparation for reading.
    /// @see #readInternal(uint8_t*, int32_t, int32_t)
    /// @see #seekInternal(int64_t)
    virtual void refill();

    /// Implements buffer refill.  Reads bytes from the current position in the input.
    /// @param b the array to read bytes into.
    /// @param offset the offset in the array to start storing bytes.
    /// @param length the number of bytes to read.
    virtual void readInternal(uint8_t* b, int32_t offset, int32_t length) = 0;

    /// Implements seek.  Sets current position in this file, where the next {@link
    /// #readInternal(uint8_t*, int32_t, int32_t)} will occur.
    /// @param pos position to set next write.
    /// @see #readInternal(uint8_t*, int32_t, int32_t)
    virtual void seekInternal(int64_t pos) = 0;
};

}

#endif
