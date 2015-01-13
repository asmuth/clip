/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ByteSliceReader.h"
#include "DocumentsWriter.h"
#include "IndexOutput.h"
#include "MiscUtils.h"

namespace Lucene {

ByteSliceReader::ByteSliceReader() {
    bufferUpto = 0;
    upto = 0;
    limit = 0;
    level = 0;
    bufferOffset = 0;
    endIndex = 0;
}

ByteSliceReader::~ByteSliceReader() {
}

void ByteSliceReader::init(const ByteBlockPoolPtr& pool, int32_t startIndex, int32_t endIndex) {
    BOOST_ASSERT(endIndex - startIndex >= 0);
    BOOST_ASSERT(startIndex >= 0);
    BOOST_ASSERT(endIndex >= 0);

    this->pool = pool;
    this->endIndex = endIndex;

    level = 0;
    bufferUpto = startIndex / DocumentsWriter::BYTE_BLOCK_SIZE;
    bufferOffset = bufferUpto * DocumentsWriter::BYTE_BLOCK_SIZE;
    buffer = pool->buffers[bufferUpto];
    upto = startIndex & DocumentsWriter::BYTE_BLOCK_MASK;

    int32_t firstSize = ByteBlockPool::levelSizeArray[0];

    if (startIndex + firstSize >= endIndex) {
        // There is only this one slice to read
        limit = endIndex & DocumentsWriter::BYTE_BLOCK_MASK;
    } else {
        limit = upto + firstSize - 4;
    }
}

bool ByteSliceReader::eof() {
    BOOST_ASSERT(upto + bufferOffset <= endIndex);
    return (upto + bufferOffset == endIndex);
}

uint8_t ByteSliceReader::readByte() {
    BOOST_ASSERT(!eof());
    BOOST_ASSERT(upto <= limit);
    if (upto == limit) {
        nextSlice();
    }
    return buffer[upto++];
}

int64_t ByteSliceReader::writeTo(const IndexOutputPtr& out) {
    int64_t size = 0;
    while (true) {
        if (limit + bufferOffset == endIndex) {
            BOOST_ASSERT(endIndex - bufferOffset >= upto);
            out->writeBytes(buffer.get(), upto, limit - upto);
            size += limit - upto;
            break;
        } else {
            out->writeBytes(buffer.get(), upto, limit - upto);
            size += limit-upto;
            nextSlice();
        }
    }
    return size;
}

void ByteSliceReader::nextSlice() {
    // Skip to our next slice
    int32_t nextIndex = ((buffer[limit] & 0xff) << 24) + ((buffer[1 + limit] & 0xff) << 16) +
                        ((buffer[2 + limit] & 0xff) << 8) + (buffer[3 + limit] & 0xff);

    level = ByteBlockPool::nextLevelArray[level];
    int32_t newSize = ByteBlockPool::levelSizeArray[level];

    bufferUpto = nextIndex / DocumentsWriter::BYTE_BLOCK_SIZE;
    bufferOffset = bufferUpto * DocumentsWriter::BYTE_BLOCK_SIZE;

    this->buffer = pool->buffers[bufferUpto];
    upto = nextIndex & DocumentsWriter::BYTE_BLOCK_MASK;

    if (nextIndex + newSize >= endIndex) {
        // We are advancing to the final slice
        BOOST_ASSERT(endIndex - nextIndex > 0);
        limit = endIndex - bufferOffset;
    } else {
        // This is not the final slice (subtract 4 for the forwarding address at the end of this new slice)
        limit = upto + newSize - 4;
    }
}

void ByteSliceReader::readBytes(uint8_t* b, int32_t offset, int32_t length) {
    while (length > 0) {
        int32_t numLeft = limit - upto;
        if (numLeft < length) {
            // Read entire slice
            MiscUtils::arrayCopy(buffer.get(), upto, b, offset, numLeft);
            offset += numLeft;
            length -= numLeft;
            nextSlice();
        } else {
            // This slice is the last one
            MiscUtils::arrayCopy(buffer.get(), upto, b, offset, length);
            upto += length;
            break;
        }
    }
}

int64_t ByteSliceReader::getFilePointer() {
    boost::throw_exception(RuntimeException(L"not implemented"));
    return 0;
}

int64_t ByteSliceReader::length() {
    boost::throw_exception(RuntimeException(L"not implemented"));
    return 0;
}

void ByteSliceReader::seek(int64_t pos) {
    boost::throw_exception(RuntimeException(L"not implemented"));
}

void ByteSliceReader::close() {
    boost::throw_exception(RuntimeException(L"not implemented"));
}

}
