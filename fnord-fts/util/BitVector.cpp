/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BitVector.h"
#include "Directory.h"
#include "IndexInput.h"
#include "IndexOutput.h"
#include "TestPoint.h"
#include "MiscUtils.h"

namespace Lucene {

const uint8_t BitVector::BYTE_COUNTS[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

BitVector::BitVector(int32_t n) {
    _size = n;
    bits = ByteArray::newInstance((_size >> 3) + 1);
    MiscUtils::arrayFill(bits.get(), 0, bits.size(), 0);
    _count = 0;
}

BitVector::BitVector(ByteArray bits, int32_t size) {
    this->bits = bits;
    this->_size = size;
    this->_count = -1;
}

BitVector::BitVector(const DirectoryPtr& d, const String& name) {
    IndexInputPtr input(d->openInput(name));
    LuceneException finally;
    try {
        _size = input->readInt(); // read size
        if (_size == -1) {
            readDgaps(input);
        } else {
            readBits(input);
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    input->close();
    finally.throwException();
}

BitVector::~BitVector() {
}

LuceneObjectPtr BitVector::clone(const LuceneObjectPtr& other) {
    ByteArray copyBits(ByteArray::newInstance(bits.size()));
    MiscUtils::arrayCopy(bits.get(), 0, copyBits.get(), 0, bits.size());
    BitVectorPtr clone = newLucene<BitVector>(copyBits, _size);
    clone->_count = _count;
    return clone;
}

void BitVector::set(int32_t bit) {
    if (bit >= _size) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    bits[bit >> 3] |= 1 << (bit & 7);
    _count = -1;
}

bool BitVector::getAndSet(int32_t bit) {
    if (bit >= _size) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    int32_t pos = (bit >> 3);
    int32_t v = bits[pos];
    int32_t flag = 1 << (bit & 7);
    if ((flag & v) != 0) {
        return true;
    } else {
        bits[pos] = (uint8_t)(v | flag);
        if (_count != -1) {
            ++_count;
        }
        return false;
    }
}

void BitVector::clear(int32_t bit) {
    if (bit >= _size) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    bits[bit >> 3] &= ~(1 << (bit & 7));
    _count = -1;
}

bool BitVector::get(int32_t bit) {
    BOOST_ASSERT(bit >= 0 && bit < _size);
    return (bits[bit >> 3] & (1 << (bit & 7))) != 0;
}

int32_t BitVector::size() {
    return _size;
}

int32_t BitVector::count() {
    // if the vector has been modified
    if (_count == -1) {
        int32_t c = 0;
        int32_t end = bits.size();
        for (int32_t i = 0; i < end; ++i) {
            c += BYTE_COUNTS[bits[i] & 0xff];    // sum bits per byte
        }
        _count = c;
    }
    return _count;
}

int32_t BitVector::getRecomputedCount() {
    int32_t c = 0;
    int32_t end = bits.size();
    for (int32_t i = 0; i < end; ++i) {
        c += BYTE_COUNTS[bits[i] & 0xff];    // sum bits per byte
    }
    return c;
}

void BitVector::write(const DirectoryPtr& d, const String& name) {
    TestScope testScope(L"BitVector", L"write");
    IndexOutputPtr output(d->createOutput(name));
    LuceneException finally;
    try {
        if (isSparse()) {
            writeDgaps(output);    // sparse bit-set more efficiently saved as d-gaps.
        } else {
            writeBits(output);
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    output->close();
    finally.throwException();
}

void BitVector::writeBits(const IndexOutputPtr& output) {
    output->writeInt(size()); // write size
    output->writeInt(count()); // write count
    output->writeBytes(bits.get(), bits.size());
}

void BitVector::writeDgaps(const IndexOutputPtr& output) {
    output->writeInt(-1); // mark using d-gaps
    output->writeInt(size()); // write size
    output->writeInt(count()); // write count
    int32_t last = 0;
    int32_t n = count();
    int32_t m = bits.size();
    for (int32_t i = 0; i < m && n > 0; ++i) {
        if (bits[i] != 0) {
            output->writeVInt(i-last);
            output->writeByte(bits[i]);
            last = i;
            n -= BYTE_COUNTS[bits[i] & 0xff];
        }
    }
}

bool BitVector::isSparse() {
    // note: order of comparisons below set to favor smaller values (no binary range search.)
    // note: adding 4 because we start with ((int) -1) to indicate d-gaps format.
    // note: we write the d-gap for the byte number, and the byte (bits[i]) itself, therefore
    //       multiplying count by (8+8) or (8+16) or (8+24) etc.:
    //       - first 8 for writing bits[i] (1 byte vs. 1 bit), and
    //       - second part for writing the byte-number d-gap as vint.
    // note: factor is for read/write of byte-arrays being faster than vints.
    int32_t factor = 10;
    if (bits.size() < (1 << 7)) {
        return factor * (4 + (8 + 8) * count()) < size();
    }
    if (bits.size() < (1 << 14)) {
        return factor * (4 + (8 + 16) * count()) < size();
    }
    if (bits.size() < (1 << 21)) {
        return factor * (4 + (8 + 24) * count()) < size();
    }
    if (bits.size() < (1 << 28)) {
        return factor * (4 + (8 + 32) * count()) < size();
    }
    return factor * (4 + (8 + 40) * count()) < size();
}

void BitVector::readBits(const IndexInputPtr& input) {
    _count = input->readInt(); // read count
    bits = ByteArray::newInstance((_size >> 3) + 1); // allocate bits
    MiscUtils::arrayFill(bits.get(), 0, bits.size(), 0);
    input->readBytes(bits.get(), 0, bits.size());
}

void BitVector::readDgaps(const IndexInputPtr& input) {
    _size = input->readInt(); // (re)read size
    _count = input->readInt(); // read count
    bits = ByteArray::newInstance((_size >> 3) + 1); // allocate bits
    MiscUtils::arrayFill(bits.get(), 0, bits.size(), 0);
    int32_t last = 0;
    int32_t n = count();
    while (n > 0) {
        last += input->readVInt();
        bits[last] = input->readByte();
        n -= BYTE_COUNTS[bits[last] & 0xff];
    }
}

BitVectorPtr BitVector::subset(int32_t start, int32_t end) {
    if (start < 0 || end > size() || end < start) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    // Special case -- return empty vector is start == end
    if (end == start) {
        return newLucene<BitVector>(0);
    }
    ByteArray bits(ByteArray::newInstance(MiscUtils::unsignedShift(end - start - 1, 3) + 1));
    int32_t s = MiscUtils::unsignedShift(start, 3);
    for (int32_t i = 0; i < bits.size(); ++i) {
        int32_t cur = 0xff & this->bits[i + s];
        int32_t next = i + s + 1 >= this->bits.size() ? 0 : 0xff & this->bits[i + s + 1];
        bits[i] = (uint8_t)(MiscUtils::unsignedShift(cur, (start & 7)) | ((next << (8 - (start & 7)))));
    }
    int32_t bitsToClear = (bits.size() * 8 - (end - start)) % 8;
    bits[bits.size() - 1] &= ~(0xff << (8 - bitsToClear));
    return newLucene<BitVector>(bits, end - start);
}

}
