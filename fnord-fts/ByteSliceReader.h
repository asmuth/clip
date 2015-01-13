/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BYTESLICEREADER_H
#define BYTESLICEREADER_H

#include "IndexInput.h"

namespace Lucene {

/// IndexInput that knows how to read the byte slices written by Posting and PostingVector.  We read the bytes in each slice
/// until we hit the end of that slice at which point we read the forwarding address of the next slice and then jump to it.
class ByteSliceReader : public IndexInput {
public:
    ByteSliceReader();
    virtual ~ByteSliceReader();

    LUCENE_CLASS(ByteSliceReader);

public:
    ByteBlockPoolPtr pool;
    int32_t bufferUpto;
    ByteArray buffer;
    int32_t upto;
    int32_t limit;
    int32_t level;
    int32_t bufferOffset;
    int32_t endIndex;

public:
    void init(const ByteBlockPoolPtr& pool, int32_t startIndex, int32_t endIndex);
    bool eof();

    /// Reads and returns a single byte.
    virtual uint8_t readByte();

    int64_t writeTo(const IndexOutputPtr& out);

    void nextSlice();

    /// Reads a specified number of bytes into an array at the specified offset.
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length);

    /// Not implemented
    virtual int64_t getFilePointer();

    /// Not implemented
    virtual int64_t length();

    /// Not implemented
    virtual void seek(int64_t pos);

    /// Not implemented
    virtual void close();
};

}

#endif
