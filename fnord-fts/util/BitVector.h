/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BITVECTOR_H
#define BITVECTOR_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Optimized implementation of a vector of bits.
class BitVector : public LuceneObject {
public:
    /// Constructs a vector capable of holding n bits.
    BitVector(int32_t n = 0);

    BitVector(ByteArray bits, int32_t size);

    /// Constructs a bit vector from the file name in Directory d,
    /// as written by the {@link #write} method.
    BitVector(const DirectoryPtr& d, const String& name);

    virtual ~BitVector();

    LUCENE_CLASS(BitVector);

protected:
    ByteArray bits;
    int32_t _size;
    int32_t _count;

    static const uint8_t BYTE_COUNTS[]; // table of bits/byte

public:
    /// Clone this vector
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Sets the value of bit to one.
    void set(int32_t bit);

    /// Sets the value of bit to true, and returns true if bit was already set.
    bool getAndSet(int32_t bit);

    /// Sets the value of bit to zero.
    void clear(int32_t bit);

    /// Returns true if bit is one and false if it is zero.
    bool get(int32_t bit);

    /// Returns the number of bits in this vector.  This is also one greater than
    /// the number of the largest valid bit number.
    int32_t size();

    /// Returns the total number of one bits in this vector.  This is efficiently
    /// computed and cached, so that, if the vector is not changed, no recomputation
    /// is done for repeated calls.
    int32_t count();

    /// For testing
    int32_t getRecomputedCount();

    /// Writes this vector to the file name in Directory d, in a format that can
    /// be read by the constructor {@link #BitVector(DirectoryPtr, const String&)}.
    void write(const DirectoryPtr& d, const String& name);

    /// Retrieve a subset of this BitVector.
    /// @param start starting index, inclusive
    /// @param end ending index, exclusive
    /// @return subset
    BitVectorPtr subset(int32_t start, int32_t end);

protected:
    /// Write as a bit set.
    void writeBits(const IndexOutputPtr& output);

    /// Write as a d-gaps list.
    void writeDgaps(const IndexOutputPtr& output);

    /// Indicates if the bit vector is sparse and should be saved as a d-gaps list,
    /// or dense, and should be saved as a bit set.
    bool isSparse();

    /// Read as a bit set.
    void readBits(const IndexInputPtr& input);

    /// Read as a d-gaps list.
    void readDgaps(const IndexInputPtr& input);
};

}

#endif
