/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "OpenBitSet.h"
#include "OpenBitSetIterator.h"
#include "BitUtil.h"
#include "MiscUtils.h"

namespace Lucene {

OpenBitSet::OpenBitSet(int64_t numBits) {
    bits = LongArray::newInstance(bits2words(numBits));
    MiscUtils::arrayFill(bits.get(), 0, bits.size(), 0LL);
    wlen = bits.size();
}

OpenBitSet::OpenBitSet(LongArray bits, int32_t numWords) {
    this->bits = bits;
    this->wlen = numWords;
}

OpenBitSet::~OpenBitSet() {
}

DocIdSetIteratorPtr OpenBitSet::iterator() {
    return newLucene<OpenBitSetIterator>(bits, wlen);
}

bool OpenBitSet::isCacheable() {
    return true;
}

int64_t OpenBitSet::capacity() {
    return bits.size() << 6;
}

int64_t OpenBitSet::size() {
    return capacity();
}

bool OpenBitSet::isEmpty() {
    return (cardinality() == 0);
}

LongArray OpenBitSet::getBits() {
    return bits;
}

void OpenBitSet::setBits(LongArray bits) {
    this->bits = bits;
}

int32_t OpenBitSet::getNumWords() {
    return wlen;
}

void OpenBitSet::setNumWords(int32_t numWords) {
    this->wlen = numWords;
}

bool OpenBitSet::get(int32_t index) {
    int32_t i = index >> 6; // div 64
    // signed shift will keep a negative index and force an array-index-out-of-bounds-exception,
    // removing the need for an explicit check.
    if (i >= bits.size()) {
        return false;
    }
    int32_t bit = (index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    return ((bits[i] & bitmask) != 0);
}

bool OpenBitSet::fastGet(int32_t index) {
    int32_t i = index >> 6; // div 64
    // signed shift will keep a negative index and force an array-index-out-of-bounds-exception,
    // removing the need for an explicit check.
    int32_t bit = (index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    return ((bits[i] & bitmask) != 0);
}

bool OpenBitSet::get(int64_t index) {
    int32_t i = (int32_t)(index >> 6); // div 64
    if (i >= bits.size()) {
        return false;
    }
    int32_t bit = ((int32_t)index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    return ((bits[i] & bitmask) != 0);
}

bool OpenBitSet::fastGet(int64_t index) {
    int32_t i = (int32_t)(index >> 6); // div 64
    int32_t bit = ((int32_t)index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    return ((bits[i] & bitmask) != 0);
}

int32_t OpenBitSet::getBit(int32_t index) {
    int32_t i = index >> 6; // div 64
    int32_t bit = (index & 0x3f); // mod 64
    return (int32_t)MiscUtils::unsignedShift(bits[i], (int64_t)bit) & 0x01;
}

void OpenBitSet::set(int64_t index) {
    int32_t wordNum = expandingWordNum(index);
    int32_t bit = (int32_t)index & 0x3f;
    int64_t bitmask = 1LL << bit;
    bits[wordNum] |= bitmask;
}

void OpenBitSet::fastSet(int32_t index) {
    int32_t wordNum = index >> 6; // div 64
    int32_t bit = index & 0x3f;
    int64_t bitmask = 1LL << bit;
    bits[wordNum] |= bitmask;
}

void OpenBitSet::fastSet(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6);
    int32_t bit = (int32_t)index & 0x3f;
    int64_t bitmask = 1LL << bit;
    bits[wordNum] |= bitmask;
}

void OpenBitSet::set(int64_t startIndex, int64_t endIndex) {
    if (endIndex <= startIndex) {
        return;
    }

    int32_t startWord = (int32_t)(startIndex >> 6);

    // since endIndex is one past the end, this is index of the last word to be changed
    int32_t endWord = expandingWordNum(endIndex - 1);

    int64_t startmask = -1LL << (startIndex & 0x3f);
    int64_t endmask = MiscUtils::unsignedShift(-1LL, -endIndex);

    if (startWord == endWord) {
        bits[startWord] |= (startmask & endmask);
        return;
    }

    bits[startWord] |= startmask;
    MiscUtils::arrayFill(bits.get(), startWord + 1, endWord, -1LL);
    bits[endWord] |= endmask;
}

int32_t OpenBitSet::expandingWordNum(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6);
    if (wordNum >= wlen) {
        ensureCapacity(index + 1);
        wlen = wordNum + 1;
    }
    return wordNum;
}

void OpenBitSet::fastClear(int32_t index) {
    int32_t wordNum = index >> 6;
    int32_t bit = (index & 0x03f);
    int64_t bitmask = 1LL << bit;
    bits[wordNum] &= ~bitmask;
}

void OpenBitSet::fastClear(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6);
    int32_t bit = (int32_t)index & 0x3f;
    int64_t bitmask = 1LL << bit;
    bits[wordNum] &= ~bitmask;
}

void OpenBitSet::clear(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6);
    if (wordNum >= wlen) {
        return;
    }
    int32_t bit = (int32_t)index & 0x3f;
    int64_t bitmask = 1LL << bit;
    bits[wordNum] &= ~bitmask;
}

void OpenBitSet::clear(int32_t startIndex, int32_t endIndex) {
    if (endIndex <= startIndex) {
        return;
    }

    int32_t startWord = (startIndex >> 6);
    if (startWord >= wlen) {
        return;
    }

    // since endIndex is one past the end, this is index of the last word to be changed.
    int32_t endWord = ((endIndex - 1) >> 6);

    int64_t startmask = -1LL << (startIndex & 0x3f);
    int64_t endmask = MiscUtils::unsignedShift(-1LL, (int64_t)-endIndex);

    // invert masks since we are clearing
    startmask = ~startmask;
    endmask = ~endmask;

    if (startWord == endWord) {
        bits[startWord] &= (startmask | endmask);
        return;
    }

    bits[startWord] &= startmask;

    int32_t middle = std::min(wlen, endWord);
    MiscUtils::arrayFill(bits.get(), startWord + 1, middle, 0LL);
    if (endWord < wlen) {
        bits[endWord] &= endmask;
    }
}

void OpenBitSet::clear(int64_t startIndex, int64_t endIndex) {
    if (endIndex <= startIndex) {
        return;
    }

    int32_t startWord = (int32_t)(startIndex>>6);
    if (startWord >= wlen) {
        return;
    }

    // since endIndex is one past the end, this is index of the last word to be changed.
    int32_t endWord = (int32_t)((endIndex - 1) >> 6);

    int64_t startmask = -1LL << (startIndex & 0x3f);
    int64_t endmask = MiscUtils::unsignedShift(-1LL, -endIndex);

    // invert masks since we are clearing
    startmask = ~startmask;
    endmask = ~endmask;

    if (startWord == endWord) {
        bits[startWord] &= (startmask | endmask);
        return;
    }

    bits[startWord] &= startmask;

    int32_t middle = std::min(wlen, endWord);
    MiscUtils::arrayFill(bits.get(), startWord + 1, middle, 0LL);
    if (endWord < wlen) {
        bits[endWord] &= endmask;
    }
}

bool OpenBitSet::getAndSet(int32_t index) {
    int32_t wordNum = index >> 6; // div 64
    int32_t bit = (index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    bool val = ((bits[wordNum] & bitmask) != 0);
    bits[wordNum] |= bitmask;
    return val;
}

bool OpenBitSet::getAndSet(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6); // div 64
    int32_t bit = (int32_t)index & 0x3f; // mod 64
    int64_t bitmask = 1LL << bit;
    bool val = ((bits[wordNum] & bitmask) != 0);
    bits[wordNum] |= bitmask;
    return val;
}

void OpenBitSet::fastFlip(int32_t index) {
    int32_t wordNum = index >> 6; // div 64
    int32_t bit = (index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    bits[wordNum] ^= bitmask;
}

void OpenBitSet::fastFlip(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6); // div 64
    int32_t bit = (int32_t)index & 0x3f; // mod 64
    int64_t bitmask = 1LL << bit;
    bits[wordNum] ^= bitmask;
}

void OpenBitSet::flip(int64_t index) {
    int32_t wordNum = expandingWordNum(index);
    int32_t bit = (int32_t)index & 0x3f; // mod 64
    int64_t bitmask = 1LL << bit;
    bits[wordNum] ^= bitmask;
}

bool OpenBitSet::flipAndGet(int32_t index) {
    int32_t wordNum = index >> 6; // div 64
    int32_t bit = (index & 0x3f); // mod 64
    int64_t bitmask = 1LL << bit;
    bits[wordNum] ^= bitmask;
    return ((bits[wordNum] & bitmask) != 0);
}

bool OpenBitSet::flipAndGet(int64_t index) {
    int32_t wordNum = (int32_t)(index >> 6); // div 64
    int32_t bit = (int32_t)index & 0x3f; // mod 64
    int64_t bitmask = 1LL << bit;
    bits[wordNum] ^= bitmask;
    return ((bits[wordNum] & bitmask) != 0);
}

void OpenBitSet::flip(int64_t startIndex, int64_t endIndex) {
    if (endIndex <= startIndex) {
        return;
    }
    int32_t startWord = (int32_t)(startIndex >> 6);

    // since endIndex is one past the end, this is index of the last word to be changed.
    int32_t endWord = expandingWordNum(endIndex - 1);

    int64_t startmask = -1LL << (startIndex & 0x3f);
    int64_t endmask = MiscUtils::unsignedShift(-1LL, -endIndex);

    if (startWord == endWord) {
        bits[startWord] ^= (startmask & endmask);
        return;
    }

    bits[startWord] ^= startmask;

    for (int32_t i = startWord + 1; i < endWord; ++i) {
        bits[i] = ~bits[i];
    }
    bits[endWord] ^= endmask;
}

int64_t OpenBitSet::cardinality() {
    return BitUtil::pop_array(bits.get(), 0, wlen);
}

int64_t OpenBitSet::intersectionCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b) {
    return BitUtil::pop_intersect(a->bits.get(), b->bits.get(), 0, std::min(a->wlen, b->wlen));
}

int64_t OpenBitSet::unionCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b) {
    int64_t tot = BitUtil::pop_union(a->bits.get(), b->bits.get(), 0, std::min(a->wlen, b->wlen));
    if (a->wlen < b->wlen) {
        tot += BitUtil::pop_array(b->bits.get(), a->wlen, b->wlen - a->wlen);
    } else if (a->wlen > b->wlen) {
        tot += BitUtil::pop_array(a->bits.get(), b->wlen, a->wlen - b->wlen);
    }
    return tot;
}

int64_t OpenBitSet::andNotCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b) {
    int64_t tot = BitUtil::pop_andnot(a->bits.get(), b->bits.get(), 0, std::min(a->wlen, b->wlen));
    if (a->wlen > b->wlen) {
        tot += BitUtil::pop_array(a->bits.get(), b->wlen, a->wlen - b->wlen);
    }
    return tot;
}

int64_t OpenBitSet::xorCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b) {
    int64_t tot = BitUtil::pop_xor(a->bits.get(), b->bits.get(), 0, std::min(a->wlen, b->wlen));
    if (a->wlen < b->wlen) {
        tot += BitUtil::pop_array(b->bits.get(), a->wlen, b->wlen - a->wlen);
    } else if (a->wlen > b->wlen) {
        tot += BitUtil::pop_array(a->bits.get(), b->wlen, a->wlen - b->wlen);
    }
    return tot;
}

int32_t OpenBitSet::nextSetBit(int32_t index) {
    int32_t i = MiscUtils::unsignedShift(index, 6);
    if (i >= wlen) {
        return -1;
    }
    int32_t subIndex = (index & 0x3f); // index within the word
    int64_t word = MiscUtils::unsignedShift(bits[i], (int64_t)subIndex); // skip all the bits to the right of index

    if (word != 0) {
        return (i << 6) + subIndex + BitUtil::ntz(word);
    }

    while (++i < wlen) {
        word = bits[i];
        if (word != 0) {
            return (i << 6) + BitUtil::ntz(word);
        }
    }

    return -1;
}

int64_t OpenBitSet::nextSetBit(int64_t index) {
    int32_t i = (int32_t)(index >> 6);
    if (i >= wlen) {
        return -1;
    }
    int32_t subIndex = (int32_t)index & 0x3f; // index within the word
    int64_t word = bits[i] >> subIndex; // skip all the bits to the right of index

    if (word != 0) {
        return ((int64_t)i << 6) + (subIndex + BitUtil::ntz(word));
    }

    while (++i < wlen) {
        word = bits[i];
        if (word != 0) {
            return ((int64_t)i << 6) + BitUtil::ntz(word);
        }
    }

    return -1;
}

LuceneObjectPtr OpenBitSet::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<OpenBitSet>();
    OpenBitSetPtr cloneSet(boost::dynamic_pointer_cast<OpenBitSet>(LuceneObject::clone(clone)));
    cloneSet->wlen = wlen;
    cloneSet->bits = LongArray::newInstance(bits.size());
    MiscUtils::arrayCopy(bits.get(), 0, cloneSet->bits.get(), 0, bits.size());
    return cloneSet;
}

void OpenBitSet::intersect(const OpenBitSetPtr& other) {
    int32_t newLen= std::min(this->wlen, other->wlen);
    LongArray thisArr = this->bits;
    LongArray otherArr = other->bits;
    // testing against zero can be more efficient
    int32_t pos = newLen;
    while (--pos >= 0) {
        thisArr[pos] &= otherArr[pos];
    }
    if (this->wlen > newLen) {
        // fill zeros from the new shorter length to the old length
        MiscUtils::arrayFill(bits.get(), newLen, this->wlen, 0LL);
    }
    this->wlen = newLen;
}

void OpenBitSet::_union(const OpenBitSetPtr& other) {
    int32_t newLen = std::max(wlen, other->wlen);
    ensureCapacityWords(newLen);

    LongArray thisArr = this->bits;
    LongArray otherArr = other->bits;
    int32_t pos = std::min(wlen, other->wlen);
    while (--pos >= 0) {
        thisArr[pos] |= otherArr[pos];
    }
    if (this->wlen < newLen) {
        MiscUtils::arrayCopy(otherArr.get(), this->wlen, thisArr.get(), this->wlen, newLen - this->wlen);
    }
    this->wlen = newLen;
}

void OpenBitSet::remove(const OpenBitSetPtr& other) {
    int32_t idx = std::min(wlen, other->wlen);
    LongArray thisArr = this->bits;
    LongArray otherArr = other->bits;
    while (--idx >= 0) {
        thisArr[idx] &= ~otherArr[idx];
    }
}

void OpenBitSet::_xor(const OpenBitSetPtr& other) {
    int32_t newLen = std::max(wlen, other->wlen);
    ensureCapacityWords(newLen);

    LongArray thisArr = this->bits;
    LongArray otherArr = other->bits;
    int32_t pos = std::min(wlen, other->wlen);
    while (--pos >= 0) {
        thisArr[pos] ^= otherArr[pos];
    }
    if (this->wlen < newLen) {
        MiscUtils::arrayCopy(otherArr.get(), this->wlen, thisArr.get(), this->wlen, newLen - this->wlen);
    }
    this->wlen = newLen;
}

void OpenBitSet::_and(const OpenBitSetPtr& other) {
    intersect(other);
}

void OpenBitSet::_or(const OpenBitSetPtr& other) {
    _union(other);
}

void OpenBitSet::andNot(const OpenBitSetPtr& other) {
    remove(other);
}

bool OpenBitSet::intersects(const OpenBitSetPtr& other) {
    int32_t pos = std::min(this->wlen, other->wlen);
    LongArray thisArr = this->bits;
    LongArray otherArr = other->bits;
    while (--pos >= 0) {
        if ((thisArr[pos] & otherArr[pos]) !=0 ) {
            return true;
        }
    }
    return false;
}

void OpenBitSet::ensureCapacityWords(int32_t numWords) {
    int32_t length = bits.size();
    if (length < numWords) {
        bits.resize(MiscUtils::getNextSize(numWords));
        MiscUtils::arrayFill(bits.get(), length, bits.size(), 0LL);
    }
}

void OpenBitSet::ensureCapacity(int64_t numBits) {
    ensureCapacityWords(bits2words(numBits));
}

void OpenBitSet::trimTrailingZeros() {
    int32_t idx = wlen - 1;
    while (idx >= 0 && bits[idx] == 0) {
        --idx;
    }
    wlen = idx + 1;
}

int32_t OpenBitSet::bits2words(int64_t numBits) {
    return (int32_t)(MiscUtils::unsignedShift(numBits - 1, (int64_t)6) + 1);
}

bool OpenBitSet::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    OpenBitSetPtr otherBitSet(boost::dynamic_pointer_cast<OpenBitSet>(other));
    if (!otherBitSet) {
        return false;
    }
    OpenBitSetPtr a;
    OpenBitSetPtr b = otherBitSet;
    // make a the larger set
    if (b->wlen > this->wlen) {
        a = b;
        b = shared_from_this();
    } else {
        a = shared_from_this();
    }

    // check for any set bits out of the range of b
    for (int32_t i = a->wlen - 1; i >= b->wlen; --i) {
        if (a->bits[i] !=0 ) {
            return false;
        }
    }

    for (int32_t i = b->wlen - 1; i >= 0; --i) {
        if (a->bits[i] != b->bits[i]) {
            return false;
        }
    }

    return true;
}

int32_t OpenBitSet::hashCode() {
    // Start with a zero hash and use a mix that results in zero if the input is zero.
    // This effectively truncates trailing zeros without an explicit check.
    int64_t hash = 0;
    for (int32_t i = bits.size(); --i >= 0;) {
        hash ^= bits[i];
        hash = (hash << 1) | MiscUtils::unsignedShift(hash, (int64_t)63); // rotate left
    }
    // Fold leftmost bits into right and add a constant to prevent empty sets from
    // returning 0, which is too common.
    return (int32_t)((hash >> 32) ^ hash) + 0x98761234;
}

}
