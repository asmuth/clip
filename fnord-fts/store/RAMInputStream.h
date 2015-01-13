/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RAMINPUTSTREAM_H
#define RAMINPUTSTREAM_H

#include "IndexInput.h"

namespace Lucene {

/// A memory-resident {@link IndexInput} implementation.
class RAMInputStream : public IndexInput {
public:
    RAMInputStream();
    RAMInputStream(const RAMFilePtr& f);
    virtual ~RAMInputStream();

    LUCENE_CLASS(RAMInputStream);

public:
    static const int32_t BUFFER_SIZE;

protected:
    RAMFilePtr file;
    int64_t _length;
    ByteArray currentBuffer;
    int32_t currentBufferIndex;
    int32_t bufferPosition;
    int64_t bufferStart;
    int32_t bufferLength;

public:
    /// Closes the stream to further operations.
    virtual void close();

    /// The number of bytes in the file.
    virtual int64_t length();

    /// Reads and returns a single byte.
    /// @see IndexOutput#writeByte(uint8_t)
    virtual uint8_t readByte();

    /// Reads a specified number of bytes into an array at the specified offset.
    /// @param b the array to read bytes into.
    /// @param offset the offset in the array to start storing bytes.
    /// @param length the number of bytes to read.
    /// @see IndexOutput#writeBytes(const uint8_t*,int)
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length);

    /// Returns the current position in this file, where the next read will occur.
    /// @see #seek(int64_t)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next read will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// Returns a clone of this stream.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

protected:
    void switchCurrentBuffer(bool enforceEOF);
};

}

#endif
