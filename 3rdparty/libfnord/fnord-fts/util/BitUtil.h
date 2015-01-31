/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BITUTIL_H
#define BITUTIL_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// A variety of high efficiency bit twiddling routines.
class BitUtil : public LuceneObject {
public:
    virtual ~BitUtil();
    LUCENE_CLASS(BitUtil);

public:
    /// Table of number of trailing zeros in a byte
    static const uint8_t ntzTable[];

public:
    /// Returns the number of bits set in the long
    static int32_t pop(int64_t x);

    /// Returns the number of set bits in an array of longs.
    static int64_t pop_array(const int64_t* A, int32_t wordOffset, int32_t numWords);

    /// Returns the popcount or cardinality of the two sets after an intersection.  Neither array is modified.
    static int64_t pop_intersect(const int64_t* A, const int64_t* B, int32_t wordOffset, int32_t numWords);

    /// Returns the popcount or cardinality of the union of two sets.  Neither array is modified.
    static int64_t pop_union(const int64_t* A, const int64_t* B, int32_t wordOffset, int32_t numWords);

    /// Returns the popcount or cardinality of A & ~B.  Neither array is modified.
    static int64_t pop_andnot(const int64_t* A, const int64_t* B, int32_t wordOffset, int32_t numWords);

    /// Returns the popcount or cardinality of A ^ B.  Neither array is modified.
    static int64_t pop_xor(const int64_t* A, const int64_t* B, int32_t wordOffset, int32_t numWords);

    /// Returns number of trailing zeros in a 64 bit long value.
    static int32_t ntz(int64_t val);

    /// Returns number of trailing zeros in a 32 bit int value.
    static int32_t ntz(int32_t val);

    /// Returns 0 based index of first set bit (only works for x!=0)
    /// This is an alternate implementation of ntz()
    static int32_t ntz2(int64_t x);

    /// Returns 0 based index of first set bit.
    /// This is an alternate implementation of ntz()
    static int32_t ntz3(int64_t x);

    /// Returns true if v is a power of two or zero.
    static bool isPowerOfTwo(int32_t v);

    /// Returns true if v is a power of two or zero.
    static bool isPowerOfTwo(int64_t v);

    /// Returns the next highest power of two, or the current value if it's already a power of two or zero.
    static int32_t nextHighestPowerOfTwo(int32_t v);

    /// Returns the next highest power of two, or the current value if it's already a power of two or zero.
    static int64_t nextHighestPowerOfTwo(int64_t v);

protected:
    inline static void CSA(int64_t& h, int64_t& l, int64_t a, int64_t b, int64_t c);
};

}

}
#endif
