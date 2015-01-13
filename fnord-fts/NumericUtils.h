/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMERICUTILS_H
#define NUMERICUTILS_H

#include "LuceneObject.h"

namespace Lucene {

/// This is a helper class to generate prefix-encoded representations for numerical values and supplies converters
/// to represent double values as sortable integers/longs.
///
/// To quickly execute range queries in Apache Lucene, a range is divided recursively into multiple intervals for
/// searching: The center of the range is searched only with the lowest possible precision in the trie, while the
/// boundaries are matched more exactly. This reduces the number of terms dramatically.
///
/// This class generates terms to achieve this: First the numerical integer values need to be converted to strings.
/// For that integer values (32 bit or 64 bit) are made unsigned and the bits are converted to ASCII chars with each
/// 7 bit. The resulting string is sortable like the original integer value. Each value is also prefixed (in the
/// first char) by the shift value (number of bits removed) used during encoding.
///
/// To also index floating point numbers, this class supplies two methods to convert them to integer values by
/// changing their bit layout: {@link #doubleToSortableLong}, {@link #doubleToSortableInt}. You will have no precision
/// loss by converting floating point numbers to integers and back (only that the integer form is not usable). Other
/// data types like dates can easily converted to longs or ints (eg. date to long).
///
/// For easy usage, the trie algorithm is implemented for indexing inside {@link NumericTokenStream} that can index
/// int, long, and double. For querying, {@link NumericRangeQuery} and {@link NumericRangeFilter} implement the query
/// part for the same data types.
///
/// This class can also be used, to generate lexicographically sortable (according {@link std::string#compare})
/// representations of numeric data types for other usages (eg. sorting).
class NumericUtils : public LuceneObject {
public:
    virtual ~NumericUtils();

    LUCENE_CLASS(NumericUtils);

public:
    /// The default precision step used by {@link NumericField}, {@link NumericTokenStream}, {@link NumericRangeQuery},
    /// and {@link NumericRangeFilter} as default.
    static const int32_t PRECISION_STEP_DEFAULT;

    /// Longs are stored at lower precision by shifting off lower bits. The shift count is stored as SHIFT_START_LONG +
    /// shift in the first character.
    static const wchar_t SHIFT_START_LONG;

    /// The maximum term length (used for char[] buffer size) for encoding long values.
    /// @see #longToPrefixCoded(long,int,char[])
    static const int32_t BUF_SIZE_LONG;

    /// Integers are stored at lower precision by shifting off lower bits. The shift count is stored as SHIFT_START_INT +
    /// shift in the first character.
    static const wchar_t SHIFT_START_INT;

    /// The maximum term length (used for char[] buffer size) for encoding int values.
    /// @see #intToPrefixCoded(int,int,char[])
    static const int32_t BUF_SIZE_INT;

public:
    /// Returns prefix coded bits after reducing the precision by shift bits.  This is method is used by
    /// {@link NumericTokenStream}.
    /// @param val the numeric value
    /// @param shift how many bits to strip from the right
    /// @param buffer that will contain the encoded chars, must be at least of {@link #BUF_SIZE_LONG} length
    /// @return number of chars written to buffer
    static int32_t longToPrefixCoded(int64_t val, int32_t shift, CharArray buffer);

    /// Returns prefix coded bits after reducing the precision by shift bits.  This is method is used by
    /// {@link LongRangeBuilder}.
    /// @param val the numeric value
    /// @param shift how many bits to strip from the right
    static String longToPrefixCoded(int64_t val, int32_t shift);

    /// This is a convenience method, that returns prefix coded bits of a long without reducing the precision.
    /// It can be used to store the full precision value as a stored field in index.
    /// To decode, use {@link #prefixCodedToLong}.
    static String longToPrefixCoded(int64_t val);

    /// Returns prefix coded bits after reducing the precision by shift bits.  This is method is used by {@link
    /// NumericTokenStream}.
    /// @param val the numeric value
    /// @param shift how many bits to strip from the right
    /// @param buffer that will contain the encoded chars, must be at least of {@link #BUF_SIZE_INT} length
    /// @return number of chars written to buffer
    static int32_t intToPrefixCoded(int32_t val, int32_t shift, CharArray buffer);

    /// Returns prefix coded bits after reducing the precision by shift bits.  This is method is used by {@link
    /// IntRangeBuilder}.
    /// @param val the numeric value
    /// @param shift how many bits to strip from the right
    static String intToPrefixCoded(int32_t val, int32_t shift);

    /// This is a convenience method, that returns prefix coded bits of an int without reducing the precision.
    /// It can be used to store the full precision value as a stored field in index.
    /// To decode, use {@link #prefixCodedToInt}.
    static String intToPrefixCoded(int32_t val);

    /// Returns a long from prefixCoded characters.  Rightmost bits will be zero for lower precision codes.
    /// This method can be used to decode eg. a stored field.
    /// @see #longToPrefixCoded(int64_t)
    static int64_t prefixCodedToLong(const String& prefixCoded);

    /// Returns an int from prefixCoded characters.  Rightmost bits will be zero for lower precision codes.
    /// This method can be used to decode eg. a stored field.
    /// @see #intToPrefixCoded(int32_t)
    static int32_t prefixCodedToInt(const String& prefixCoded);

    /// Converts a double value to a sortable signed long.  The value is converted by getting their IEEE 754
    /// floating-point "double format" bit layout and then some bits are swapped, to be able to compare the
    /// result as int64_t.  By this the precision is not reduced, but the value can easily used as a int64_t.
    /// @see #sortableLongToDouble
    static int64_t doubleToSortableLong(double val);

    /// Convenience method: this just returns: longToPrefixCoded(doubleToSortableLong(val))
    static String doubleToPrefixCoded(double val);

    /// Converts a sortable long back to a double.
    /// @see #doubleToSortableLong
    static double sortableLongToDouble(int64_t val);

    /// Convenience method: this just returns: sortableLongToDouble(prefixCodedToLong(val))
    static double prefixCodedToDouble(const String& val);

    /// Splits a int64_t range recursively.  You may implement a builder that adds clauses to a {@link BooleanQuery}
    /// for each call to its {@link LongRangeBuilder#addRange(String,String)} method.
    /// This method is used by {@link NumericRangeQuery}.
    static void splitLongRange(const LongRangeBuilderPtr& builder, int32_t precisionStep, int64_t minBound, int64_t maxBound);

    /// Splits an int32_t range recursively.  You may implement a builder that adds clauses to a {@link BooleanQuery}
    /// for each call to its {@link IntRangeBuilder#addRange(String,String)} method.
    /// This method is used by {@link NumericRangeQuery}.
    static void splitIntRange(const IntRangeBuilderPtr& builder, int32_t precisionStep, int32_t minBound, int32_t maxBound);

    /// This helper does the splitting for both 32 and 64 bit.
    static void splitRange(const LuceneObjectPtr& builder, int32_t valSize, int32_t precisionStep, int64_t minBound, int64_t maxBound);

    /// Helper that delegates to correct range builder
    static void addRange(const LuceneObjectPtr& builder, int32_t valSize, int64_t minBound, int64_t maxBound, int32_t shift);
};

/// Callback for {@link #splitLongRange}.  You need to overwrite only one of the methods.
/// NOTE: This is a very low-level interface, the method signatures may change in later versions.
class LongRangeBuilder : public LuceneObject {
public:
    virtual ~LongRangeBuilder();

public:
    /// Overwrite this method, if you like to receive the already prefix encoded range bounds. You can directly build
    /// classical (inclusive) range queries from them.
    virtual void addRange(const String& minPrefixCoded, const String& maxPrefixCoded);

    /// Overwrite this method, if you like to receive the raw long range bounds.  You can use this for eg. debugging
    /// purposes (print out range bounds).
    virtual void addRange(int64_t min, int64_t max, int32_t shift);
};

class IntRangeBuilder : public LuceneObject {
public:
    virtual ~IntRangeBuilder();

public:
    /// Overwrite this method, if you like to receive the already prefix encoded range bounds.  You can directly build
    /// classical range (inclusive) queries from them.
    virtual void addRange(const String& minPrefixCoded, const String& maxPrefixCoded);

    /// Overwrite this method, if you like to receive the raw int range bounds.  You can use this for eg. debugging
    /// purposes (print out range bounds).
    virtual void addRange(int32_t min, int32_t max, int32_t shift);
};

}

#endif
