/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BitSet.h"
#include "BitUtil.h"

namespace Lucene {

BitSet::BitSet(uint32_t size) : bitSet(size) {
}

BitSet::~BitSet() {
}

const uint64_t* BitSet::getBits() {
    return bitSet.empty() ? NULL : static_cast<const uint64_t*>(&bitSet.m_bits[0]);
}

void BitSet::clear() {
    bitSet.clear();
}

void BitSet::clear(uint32_t bitIndex) {
    if (bitIndex <= bitSet.size()) {
        bitSet.set(bitIndex, false);
    }
}

void BitSet::fastClear(uint32_t bitIndex) {
    bitSet.set(bitIndex, false);
}

void BitSet::clear(uint32_t fromIndex, uint32_t toIndex) {
    toIndex = std::min(toIndex, (uint32_t)bitSet.size());
    for (bitset_type::size_type i = std::min(fromIndex, (uint32_t)bitSet.size()); i < toIndex; ++i) {
        bitSet.set(i, false);
    }
}

void BitSet::fastClear(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, false);
    }
}

void BitSet::set(uint32_t bitIndex) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.set(bitIndex, true);
}

void BitSet::fastSet(uint32_t bitIndex) {
    bitSet.set(bitIndex, true);
}

void BitSet::set(uint32_t bitIndex, bool value) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.set(bitIndex, value);
}

void BitSet::fastSet(uint32_t bitIndex, bool value) {
    bitSet.set(bitIndex, value);
}

void BitSet::set(uint32_t fromIndex, uint32_t toIndex) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, true);
    }
}

void BitSet::fastSet(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, true);
    }
}

void BitSet::set(uint32_t fromIndex, uint32_t toIndex, bool value) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, value);
    }
}

void BitSet::fastSet(uint32_t fromIndex, uint32_t toIndex, bool value) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, value);
    }
}

void BitSet::flip(uint32_t bitIndex) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.flip(bitIndex);
}

void BitSet::fastFlip(uint32_t bitIndex) {
    bitSet.flip(bitIndex);
}

void BitSet::flip(uint32_t fromIndex, uint32_t toIndex) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.flip(i);
    }
}

void BitSet::fastFlip(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.flip(i);
    }
}

uint32_t BitSet::size() const {
    return bitSet.num_blocks() * sizeof(bitset_type::block_type) * 8;
}

uint32_t BitSet::numBlocks() const {
    return bitSet.num_blocks();
}

bool BitSet::isEmpty() const {
    return bitSet.none();
}

bool BitSet::get(uint32_t bitIndex) const {
    return bitIndex < bitSet.size() ? bitSet.test(bitIndex) : false;
}

bool BitSet::fastGet(uint32_t bitIndex) const {
    return bitSet.test(bitIndex);
}

int32_t BitSet::nextSetBit(uint32_t fromIndex) const {
    bitset_type::size_type next = fromIndex == 0 ? bitSet.find_first() : bitSet.find_next(fromIndex - 1);
    return next == bitset_type::npos ? -1 : next;
}

void BitSet::_and(const BitSetPtr& set) {
    bitset_type::size_type minBlocks = std::min(bitSet.num_blocks(), set->bitSet.num_blocks());
    for (bitset_type::size_type i = 0; i < minBlocks; ++i) {
        bitSet.m_bits[i] &= set->bitSet.m_bits[i];
    }
    if (bitSet.num_blocks() > minBlocks) {
        std::fill(bitSet.m_bits.begin() + minBlocks, bitSet.m_bits.end(), bitset_type::block_type(0));
    }
}

void BitSet::_or(const BitSetPtr& set) {
    bitset_type::size_type minBlocks = std::min(bitSet.num_blocks(), set->bitSet.num_blocks());
    if (set->bitSet.size() > bitSet.size()) {
        resize(set->bitSet.size());
    }
    for (bitset_type::size_type i = 0; i < minBlocks; ++i) {
        bitSet.m_bits[i] |= set->bitSet.m_bits[i];
    }
    if (bitSet.num_blocks() > minBlocks) {
        std::copy(set->bitSet.m_bits.begin() + minBlocks, set->bitSet.m_bits.end(), bitSet.m_bits.begin() + minBlocks);
    }
}

void BitSet::_xor(const BitSetPtr& set) {
    bitset_type::size_type minBlocks = std::min(bitSet.num_blocks(), set->bitSet.num_blocks());
    if (set->bitSet.size() > bitSet.size()) {
        resize(set->bitSet.size());
    }
    for (bitset_type::size_type i = 0; i < minBlocks; ++i) {
        bitSet.m_bits[i] ^= set->bitSet.m_bits[i];
    }
    if (bitSet.num_blocks() > minBlocks) {
        std::copy(set->bitSet.m_bits.begin() + minBlocks, set->bitSet.m_bits.end(), bitSet.m_bits.begin() + minBlocks);
    }
}

void BitSet::andNot(const BitSetPtr& set) {
    bitset_type::size_type minBlocks = std::min(bitSet.num_blocks(), set->bitSet.num_blocks());
    for (bitset_type::size_type i = 0; i < minBlocks; ++i) {
        bitSet.m_bits[i] &= ~set->bitSet.m_bits[i];
    }
}

bool BitSet::intersectsBitSet(const BitSetPtr& set) const {
    return bitSet.intersects(set->bitSet);
}

uint32_t BitSet::cardinality() {
    return bitSet.num_blocks() == 0 ? 0 : (uint32_t)BitUtil::pop_array((int64_t*)getBits(), 0, bitSet.num_blocks());
}

void BitSet::resize(uint32_t size) {
    bitset_type::size_type old_num_blocks = bitSet.num_blocks();
    bitset_type::size_type required_blocks = bitSet.calc_num_blocks(size);
    if (required_blocks != old_num_blocks) {
        bitSet.m_bits.resize(required_blocks, bitset_type::block_type(0));
    }
    bitSet.m_num_bits = size;
    uint64_t extra_bits = static_cast<uint64_t>(bitSet.size() % bitSet.bits_per_block);
    if (extra_bits != 0) {
        bitSet.m_bits.back() &= ~(~static_cast<bitset_type::block_type>(0) << extra_bits);
    }
}

bool BitSet::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    BitSetPtr otherBitSet(boost::dynamic_pointer_cast<BitSet>(other));
    if (!otherBitSet) {
        return false;
    }
    BitSetPtr first = bitSet.num_blocks() < otherBitSet->bitSet.num_blocks() ? otherBitSet : shared_from_this();
    BitSetPtr second = bitSet.num_blocks() < otherBitSet->bitSet.num_blocks() ? shared_from_this() : otherBitSet;
    bitset_type::size_type firstLength = first->bitSet.num_blocks();
    bitset_type::size_type secondLength = second->bitSet.num_blocks();
    for (bitset_type::size_type i = secondLength; i < firstLength; ++i) {
        if (first->bitSet.m_bits[i] != 0) {
            return false;
        }
    }
    for (bitset_type::size_type i = 0; i < secondLength; ++i) {
        if (first->bitSet.m_bits[i] != second->bitSet.m_bits[i]) {
            return false;
        }
    }
    return true;
}

int32_t BitSet::hashCode() {
    // Start with a zero hash and use a mix that results in zero if the input is zero.
    // This effectively truncates trailing zeros without an explicit check.
    int64_t hash = 0;
    uint32_t maxSize = bitSet.num_blocks();
    const uint64_t* bits = getBits();
    for (uint32_t bit = 0; bit < maxSize; ++bit) {
        hash ^= bits[bit];
        hash = (hash << 1) | (hash >> 63); // rotate left
    }
    // Fold leftmost bits into right and add a constant to prevent empty sets from
    // returning 0, which is too common.
    return (int32_t)((hash >> 32) ^ hash) + 0x98761234;
}

LuceneObjectPtr BitSet::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<BitSet>();
    BitSetPtr cloneBitSet(boost::dynamic_pointer_cast<BitSet>(LuceneObject::clone(clone)));
    cloneBitSet->bitSet = bitSet;
    return cloneBitSet;
}

}
