/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RAMOUTPUTSTREAM_H
#define RAMOUTPUTSTREAM_H

#include "IndexOutput.h"

namespace Lucene {

/// A memory-resident {@link IndexOutput} implementation.
class RAMOutputStream : public IndexOutput {
public:
    /// Construct an empty output buffer.
    RAMOutputStream();
    RAMOutputStream(const RAMFilePtr& f);
    virtual ~RAMOutputStream();

    LUCENE_CLASS(RAMOutputStream);

public:
    static const int32_t BUFFER_SIZE;

protected:
    RAMFilePtr file;
    ByteArray currentBuffer;
    int32_t currentBufferIndex;
    int32_t bufferPosition;
    int64_t bufferStart;
    int32_t bufferLength;

public:
    /// Copy the current contents of this buffer to the named output.
    void writeTo(const IndexOutputPtr& out);

    /// Resets this to an empty file.
    void reset();

    /// Closes this stream to further operations.
    virtual void close();

    /// Sets current position in this file, where the next write will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// The number of bytes in the file.
    virtual int64_t length();

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

    /// Returns the current position in this file, where the next write will occur.
    virtual int64_t getFilePointer();

    /// Returns byte usage of all buffers.
    int64_t sizeInBytes();

protected:
    void switchCurrentBuffer();
    void setFileLength();
};

}

#endif
