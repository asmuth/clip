/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef OPENBITSET_H
#define OPENBITSET_H

#include "DocIdSet.h"

namespace Lucene {

/// An "open" BitSet implementation that allows direct access to the array of words storing the bits.
///
/// The goals of OpenBitSet are the fastest implementation possible, and maximum code reuse.  Extra
/// safety and encapsulation may always be built on top, but if that's built in, the cost can never
/// be removed (and hence people re-implement their own version in order to get better performance).
class OpenBitSet : public DocIdSet {
public:
    /// Constructs an OpenBitSet large enough to hold numBits.
    OpenBitSet(int64_t numBits = 64);

    /// Constructs an OpenBitSet from an existing LongArray.
    ///
    /// The first 64 bits are in long[0], with bit index 0 at the least significant bit, and bit
    /// index 63 at the most significant.  Given a bit index, the word containing it is long[index/64],
    /// and it is at bit number index%64 within that word.
    ///
    /// numWords are the number of elements in the array that contain set bits (non-zero longs).
    /// numWords should be <= bits.length(), and any existing words in the array at position >=
    /// numWords should be zero.
    OpenBitSet(LongArray bits, int32_t numWords);

    virtual ~OpenBitSet();

    LUCENE_CLASS(OpenBitSet);

protected:
    LongArray bits;
    int32_t wlen; // number of words (elements) used in the array

public:
    virtual DocIdSetIteratorPtr iterator();

    /// This DocIdSet implementation is cacheable.
    virtual bool isCacheable();

    /// Returns the current capacity in bits (1 greater than the index of the last bit)
    int64_t capacity();

    /// Returns the current capacity of this set.  Included for compatibility.  This is *not*
    /// equal to {@link #cardinality}
    int64_t size();

    /// Returns true if there are no set bits
    bool isEmpty();

    /// Returns the long[] storing the bits
    LongArray getBits();

    /// Sets a new long[] to use as the bit storage
    void setBits(LongArray bits);

    /// Gets the number of longs in the array that are in use
    int32_t getNumWords();

    /// Sets the number of longs in the array that are in use
    void setNumWords(int32_t numWords);

    /// Returns true or false for the specified bit index.
    bool get(int32_t index);

    /// Returns true or false for the specified bit index.
    /// The index should be less than the OpenBitSet size
    bool fastGet(int32_t index);

    /// Returns true or false for the specified bit index
    bool get(int64_t index);

    /// Returns true or false for the specified bit index.
    /// The index should be less than the OpenBitSet size.
    bool fastGet(int64_t index);

    /// Returns 1 if the bit is set, 0 if not.
    /// The index should be less than the OpenBitSet size
    int32_t getBit(int32_t index);

    /// Sets a bit, expanding the set size if necessary
    void set(int64_t index);

    /// Sets the bit at the specified index.
    /// The index should be less than the OpenBitSet size.
    void fastSet(int32_t index);

    /// Sets the bit at the specified index.
    /// The index should be less than the OpenBitSet size.
    void fastSet(int64_t index);

    /// Sets a range of bits, expanding the set size if necessary
    /// @param startIndex lower index
    /// @param endIndex one-past the last bit to set
    void set(int64_t startIndex, int64_t endIndex);

    /// Clears a bit.
    /// The index should be less than the OpenBitSet size.
    void fastClear(int32_t index);

    /// Clears a bit.
    /// The index should be less than the OpenBitSet size.
    void fastClear(int64_t index);

    /// Clears a bit, allowing access beyond the current set size without changing the size.
    void clear(int64_t index);

    /// Clears a range of bits.  Clearing past the end does not change the size of the set.
    /// @param startIndex lower index
    /// @param endIndex one-past the last bit to clear
    void clear(int32_t startIndex, int32_t endIndex);

    /// Clears a range of bits.  Clearing past the end does not change the size of the set.
    /// @param startIndex lower index
    /// @param endIndex one-past the last bit to clear
    void clear(int64_t startIndex, int64_t endIndex);

    /// Sets a bit and returns the previous value.
    /// The index should be less than the OpenBitSet size.
    bool getAndSet(int32_t index);

    /// Sets a bit and returns the previous value.
    /// The index should be less than the OpenBitSet size.
    bool getAndSet(int64_t index);

    /// Flips a bit.
    /// The index should be less than the OpenBitSet size.
    void fastFlip(int32_t index);

    /// Flips a bit.
    /// The index should be less than the OpenBitSet size.
    void fastFlip(int64_t index);

    /// Flips a bit, expanding the set size if necessary
    void flip(int64_t index);

    /// Flips a bit and returns the resulting bit value.
    /// The index should be less than the OpenBitSet size.
    bool flipAndGet(int32_t index);

    /// Flips a bit and returns the resulting bit value.
    /// The index should be less than the OpenBitSet size.
    bool flipAndGet(int64_t index);

    /// Flips a range of bits, expanding the set size if necessary
    /// @param startIndex lower index
    /// @param endIndex one-past the last bit to flip
    void flip(int64_t startIndex, int64_t endIndex);

    /// @return the number of set bits
    int64_t cardinality();

    /// Returns the popcount or cardinality of the intersection of the two sets.
    /// Neither set is modified.
    static int64_t intersectionCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b);

    /// Returns the popcount or cardinality of the union of the two sets.
    /// Neither set is modified.
    static int64_t unionCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b);

    /// Returns the popcount or cardinality of "a and not b" or "intersection(a, not(b))".
    /// Neither set is modified.
    static int64_t andNotCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b);

    /// Returns the popcount or cardinality of the exclusive-or of the two sets.
    /// Neither set is modified.
    static int64_t xorCount(const OpenBitSetPtr& a, const OpenBitSetPtr& b);

    /// Returns the index of the first set bit starting at the index specified.
    /// -1 is returned if there are no more set bits.
    int32_t nextSetBit(int32_t index);

    /// Returns the index of the first set bit starting at the index specified.
    /// -1 is returned if there are no more set bits.
    int64_t nextSetBit(int64_t index);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// this = this AND other
    void intersect(const OpenBitSetPtr& other);

    /// this = this OR other
    void _union(const OpenBitSetPtr& other);

    /// Remove all elements set in other. this = this AND_NOT other
    void remove(const OpenBitSetPtr& other);

    /// this = this XOR other
    void _xor(const OpenBitSetPtr& other);

    /// see {@link intersect}
    void _and(const OpenBitSetPtr& other);

    /// see {@link union}
    void _or(const OpenBitSetPtr& other);

    /// see {@link remove}
    void andNot(const OpenBitSetPtr& other);

    /// Returns true if the sets have any elements in common
    bool intersects(const OpenBitSetPtr& other);

    /// Expand the LongArray with the size given as a number of words (64 bit longs).
    /// getNumWords() is unchanged by this call.
    void ensureCapacityWords(int32_t numWords);

    /// Ensure that the LongArray is big enough to hold numBits, expanding it if necessary.
    /// getNumWords() is unchanged by this call.
    void ensureCapacity(int64_t numBits);

    /// Lowers numWords, the number of words in use, by checking for trailing zero words.
    void trimTrailingZeros();

    /// Returns the number of 64 bit words it would take to hold numBits.
    static int32_t bits2words(int64_t numBits);

    /// Returns true if both sets have the same bits set
    virtual bool equals(const LuceneObjectPtr& other);

    virtual int32_t hashCode();

protected:
    int32_t expandingWordNum(int64_t index);
};

}

#endif
