/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ByteBlockPool.h"
#include "DocumentsWriter.h"
#include "MiscUtils.h"

namespace Lucene {

// Size of each slice.  These arrays should be at most 16 elements (index is encoded with 4 bits).  First array
// is just a compact way to encode X+1 with a max.  Second array is the length of each slice, ie first slice is
// 5 bytes, next slice is 14 bytes, etc.
const int32_t ByteBlockPool::nextLevelArray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 9};
const int32_t ByteBlockPool::levelSizeArray[] = {5, 14, 20, 30, 40, 40, 80, 80, 120, 200};

ByteBlockPool::ByteBlockPool(const ByteBlockPoolAllocatorBasePtr& allocator, bool trackAllocations) {
    buffers = Collection<ByteArray>::newInstance(10);
    bufferUpto = -1;
    byteUpto = DocumentsWriter::BYTE_BLOCK_SIZE;
    byteOffset = -DocumentsWriter::BYTE_BLOCK_SIZE;

    this->allocator = allocator;
    this->trackAllocations = trackAllocations;
}

ByteBlockPool::~ByteBlockPool() {
}

int32_t ByteBlockPool::FIRST_LEVEL_SIZE() {
    return levelSizeArray[0];
}

void ByteBlockPool::reset() {
    if (bufferUpto != -1) {
        // We allocated at least one buffer
        for (int32_t i = 0; i < bufferUpto; ++i) {
            // Fully zero fill buffers that we fully used
            MiscUtils::arrayFill(buffers[i].get(), 0, buffers[i].size(), 0);
        }

        // Partial zero fill the final buffer
        MiscUtils::arrayFill(buffers[bufferUpto].get(), 0, byteUpto, 0);

        if (bufferUpto > 0) {
            // Recycle all but the first buffer
            allocator->recycleByteBlocks(buffers, 1, 1 + bufferUpto);
        }

        // Re-use the first buffer
        bufferUpto = 0;
        byteUpto = 0;
        byteOffset = 0;
        buffer = buffers[0];
    }
}

void ByteBlockPool::nextBuffer() {
    if (1 + bufferUpto == buffers.size()) {
        buffers.resize((int32_t)((double)buffers.size() * 1.5));
    }
    buffers[1 + bufferUpto] = allocator->getByteBlock(trackAllocations);
    buffer = buffers[1 + bufferUpto];
    ++bufferUpto;

    byteUpto = 0;
    byteOffset += DocumentsWriter::BYTE_BLOCK_SIZE;
}

int32_t ByteBlockPool::newSlice(int32_t size) {
    if (byteUpto > DocumentsWriter::BYTE_BLOCK_SIZE - size) {
        nextBuffer();
    }
    int32_t upto = byteUpto;
    byteUpto += size;
    buffer[byteUpto - 1] = 16;
    return upto;
}

int32_t ByteBlockPool::allocSlice(ByteArray slice, int32_t upto) {
    int32_t level = slice[upto] & 15;
    int32_t newLevel = nextLevelArray[level];
    int32_t newSize = levelSizeArray[newLevel];

    // Maybe allocate another block
    if (byteUpto > DocumentsWriter::BYTE_BLOCK_SIZE - newSize) {
        nextBuffer();
    }

    int32_t newUpto = byteUpto;
    int32_t offset = newUpto + byteOffset;
    byteUpto += newSize;

    // Copy forward the past 3 bytes (which we are about to overwrite with the forwarding address)
    buffer[newUpto] = slice[upto - 3];
    buffer[newUpto + 1] = slice[upto - 2];
    buffer[newUpto + 2] = slice[upto - 1];

    // Write forwarding address at end of last slice
    slice[upto - 3] = (uint8_t)MiscUtils::unsignedShift(offset, 24);
    slice[upto - 2] = (uint8_t)MiscUtils::unsignedShift(offset, 16);
    slice[upto - 1] = (uint8_t)MiscUtils::unsignedShift(offset, 8);
    slice[upto] = (uint8_t)offset;

    // Write new level
    buffer[byteUpto - 1] = (uint8_t)(16 | newLevel);

    return (newUpto + 3);
}

ByteBlockPoolAllocatorBase::~ByteBlockPoolAllocatorBase() {
}

}
