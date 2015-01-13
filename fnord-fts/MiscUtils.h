/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MISCUTILS_H
#define MISCUTILS_H

#include "Lucene.h"

namespace Lucene {

class MiscUtils {
protected:
    static const uint32_t SINGLE_EXPONENT_MASK;
    static const uint32_t SINGLE_MANTISSA_MASK;
    static const uint32_t SINGLE_NAN_BITS;

    static const uint64_t DOUBLE_SIGN_MASK;
    static const uint64_t DOUBLE_EXPONENT_MASK;
    static const uint64_t DOUBLE_MANTISSA_MASK;
    static const uint64_t DOUBLE_NAN_BITS;

public:
    /// Return given time in milliseconds.
    static uint64_t getTimeMillis(boost::posix_time::ptime time);

    /// Returns the current time in milliseconds.
    static uint64_t currentTimeMillis();

    /// This over-allocates proportional to the list size, making room for additional growth.
    /// The over-allocation is mild, but is enough to give linear-time amortized behavior over a long
    /// sequence of appends().
    /// The growth pattern is:  0, 4, 8, 16, 25, 35, 46, 58, 72, 88, ...
    static int32_t getNextSize(int32_t targetSize);

    /// Only reallocate if we are "substantially" smaller.  This saves us from "running hot" (constantly
    /// making a bit bigger then a bit smaller, over and over)
    static int32_t getShrinkSize(int32_t currentSize, int32_t targetSize);

    /// Compares two byte[] arrays, element by element, and returns the number of elements common to
    /// both arrays.
    /// @param bytes1 The first byte[] to compare
    /// @param bytes2 The second byte[] to compare
    /// @return The number of common elements.
    static int32_t bytesDifference(uint8_t* bytes1, int32_t len1, uint8_t* bytes2, int32_t len2);

    template <typename TYPE>
    static int32_t hashLucene(TYPE type) {
        return type->hashCode();
    }

    template <typename TYPE>
    static int32_t hashNumeric(TYPE type) {
        return type;
    }

    template <typename ITER, typename PRED>
    static int32_t hashCode(ITER first, ITER last, PRED pred) {
        int32_t code = 0;
        for (ITER hash = first; hash != last; ++hash) {
            code = code * 31 + pred(*hash);
        }
        return code;
    }

    /// Returns hash of chars in range start (inclusive) to end (inclusive)
    static int32_t hashCode(const wchar_t* array, int32_t start, int32_t end);

    /// Returns hash of bytes in range start (inclusive) to end (inclusive)
    static int32_t hashCode(const uint8_t* array, int32_t start, int32_t end);

    /// Returns hash code of given boolean
    static int32_t hashCode(bool value);

    /// Copy elements from on buffer to another
    template <typename SOURCE, typename DEST>
    static void arrayCopy(SOURCE source, int32_t sourceOffset, DEST dest, int32_t destOffset, int32_t length) {
        std::copy(source + sourceOffset, source + sourceOffset + length, dest + destOffset);
    }

    /// Fill buffer with given element
    template <typename DEST, typename FILL>
    static void arrayFill(DEST dest, int32_t destFrom, int32_t destTo, FILL value) {
        std::fill(dest + destFrom, dest + destTo, value);
    }

    /// Returns a representation of the specified floating-point value according to the IEEE 754 floating-point
    /// "single format" bit layout.
    static int32_t doubleToIntBits(double value);

    /// Returns a representation of the specified floating-point value according to the IEEE 754 floating-point
    /// "single format" bit layout, preserving Not-a-Number (NaN) values.
    static int32_t doubleToRawIntBits(double value);

    /// Returns the float value corresponding to a given bit representation. The argument is considered to be a
    /// representation of a floating-point value according to the IEEE 754 floating-point "single format" bit layout.
    static double intBitsToDouble(int32_t bits);

    /// Returns a representation of the specified floating-point value according to the IEEE 754 floating-point
    /// "double format" bit layout.
    static int64_t doubleToLongBits(double value);

    /// Returns a representation of the specified floating-point value according to the IEEE 754 floating-point
    /// "double format" bit layout, preserving Not-a-Number (NaN) values.
    static int64_t doubleToRawLongBits(double value);

    /// Returns the double value corresponding to a given bit representation. The argument is considered to be a
    /// representation of a floating-point value according to the IEEE 754 floating-point "double format" bit layout.
    static double longBitsToDouble(int64_t bits);

    /// Returns true if the specified number is infinitely large in magnitude, false otherwise.
    static bool isInfinite(double value);

    /// Returns true if this Double value is a Not-a-Number (NaN), false otherwise.
    static bool isNaN(double value);

    /// Return whether given Lucene object is of a specified type
    template <typename TYPE>
    static bool typeOf(const LuceneObjectPtr& object) {
        return boost::dynamic_pointer_cast<TYPE>(object).get() != NULL;
    }

    /// Return whether given Lucene objects are of equal type.
    static bool equalTypes(const LuceneObjectPtr& first, const LuceneObjectPtr& second);

    /// Perform unsigned right-shift (left bits are zero filled)
    static int64_t unsignedShift(int64_t num, int64_t shift);

    /// Perform unsigned right-shift (left bits are zero filled)
    static int32_t unsignedShift(int32_t num, int32_t shift);
};

}

#endif
