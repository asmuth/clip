/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BYTEBLOCKPOOL_H
#define BYTEBLOCKPOOL_H

#include "LuceneObject.h"

namespace Lucene {

/// Class that Posting and PostingVector use to write byte streams into shared fixed-size byte[] arrays.
/// The idea is to allocate slices of increasing lengths.  For example, the first slice is 5 bytes, the
/// next slice is 14, etc.  We start by writing our bytes into the first 5 bytes.  When we hit the end of
/// the slice, we allocate the next slice and then write the address of the new slice into the last 4
/// bytes of the previous slice (the "forwarding address").
///
/// Each slice is filled with 0's initially, and we mark the end with a non-zero byte.  This way the methods
/// that are writing into the slice don't need to record its length and instead allocate a new slice once
/// they hit a non-zero byte.
class ByteBlockPool : public LuceneObject {
public:
    ByteBlockPool(const ByteBlockPoolAllocatorBasePtr& allocator, bool trackAllocations);
    virtual ~ByteBlockPool();

    LUCENE_CLASS(ByteBlockPool);

public:
    Collection<ByteArray> buffers;
    int32_t bufferUpto; // Which buffer we are up to
    int32_t byteUpto; // Where we are in head buffer

    ByteArray buffer;
    int32_t byteOffset;

    static const int32_t nextLevelArray[];
    static const int32_t levelSizeArray[];

protected:
    bool trackAllocations;
    ByteBlockPoolAllocatorBasePtr allocator;

public:
    static int32_t FIRST_LEVEL_SIZE();

    void reset();
    void nextBuffer();
    int32_t newSlice(int32_t size);
    int32_t allocSlice(ByteArray slice, int32_t upto);
};

class ByteBlockPoolAllocatorBase : public LuceneObject {
public:
    virtual ~ByteBlockPoolAllocatorBase();

    LUCENE_CLASS(ByteBlockPoolAllocatorBase);

public:
    virtual void recycleByteBlocks(Collection<ByteArray> blocks, int32_t start, int32_t end) = 0;
    virtual void recycleByteBlocks(Collection<ByteArray> blocks) = 0;
    virtual ByteArray getByteBlock(bool trackAllocations) = 0;
};

}

#endif
