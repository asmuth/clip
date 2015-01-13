/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ByteSliceWriter.h"
#include "DocumentsWriter.h"
#include "MiscUtils.h"

namespace Lucene {

ByteSliceWriter::ByteSliceWriter(const ByteBlockPoolPtr& pool) {
    this->pool = pool;
    upto = 0;
    offset0 = 0;
}

ByteSliceWriter::~ByteSliceWriter() {
}

void ByteSliceWriter::init(int32_t address) {
    slice = pool->buffers[address >> DocumentsWriter::BYTE_BLOCK_SHIFT];
    BOOST_ASSERT(slice);
    upto = (address & DocumentsWriter::BYTE_BLOCK_MASK);
    offset0 = address;
    BOOST_ASSERT(upto < slice.size());
}

void ByteSliceWriter::writeByte(uint8_t b) {
    BOOST_ASSERT(slice);
    if (slice[upto] != 0) {
        upto = pool->allocSlice(slice, upto);
        slice = pool->buffer;
        offset0 = pool->byteOffset;
        BOOST_ASSERT(slice);
    }
    slice[upto++] = b;
    BOOST_ASSERT(upto != slice.size());
}

void ByteSliceWriter::writeBytes(const uint8_t* b, int32_t offset, int32_t length) {
    int32_t offsetEnd = offset + length;
    while (offset < offsetEnd) {
        if (slice[upto] != 0) {
            // End marker
            upto = pool->allocSlice(slice, upto);
            slice = pool->buffer;
            offset0 = pool->byteOffset;
        }

        slice[upto++] = b[offset++];
        BOOST_ASSERT(upto != slice.size());
    }
}

int32_t ByteSliceWriter::getAddress() {
    return upto + (offset0 & DocumentsWriter::BYTE_BLOCK_NOT_MASK);
}

void ByteSliceWriter::writeVInt(int32_t i) {
    while ((i & ~0x7f) != 0) {
        writeByte((uint8_t)((i & 0x7f) | 0x80));
        i = MiscUtils::unsignedShift(i, 7);
    }
    writeByte((uint8_t)i);
}

}
