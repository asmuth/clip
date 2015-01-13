/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMBERTOOLS_H
#define NUMBERTOOLS_H

#include "LuceneObject.h"

namespace Lucene {

/// Provides support for converting longs to Strings, and back again. The strings are structured so that
/// lexicographic sorting order is preserved.
///
/// That is, if l1 is less than l2 for any two longs l1 and l2, then NumberTools.longToString(l1) is
/// lexicographically less than NumberTools.longToString(l2). (Similarly for "greater than" and "equals".)
///
/// This class handles all long values (unlike {@link DateField}).
///
/// @deprecated For new indexes use {@link NumericUtils} instead, which provides a sortable binary representation
/// (prefix encoded) of numeric values.
/// To index and efficiently query numeric values use {@link NumericField} and {@link NumericRangeQuery}. This
/// class is included for use with existing indices and will be removed in a future release (possibly Lucene 4.0).
class NumberTools : public LuceneObject {
public:
    virtual ~NumberTools();

    LUCENE_CLASS(NumberTools);

protected:
    static const int32_t RADIX;

    static const wchar_t NEGATIVE_PREFIX;

    // NB: NEGATIVE_PREFIX must be < POSITIVE_PREFIX
    static const wchar_t POSITIVE_PREFIX;

public:
    /// Equivalent to longToString(LLONG_MIN)
    static const String& MIN_STRING_VALUE();

    /// Equivalent to longToString(LLONG_MAX)
    static const String& MAX_STRING_VALUE();

    /// The length of (all) strings returned by {@link #longToString}
    static int32_t STR_SIZE();

    /// Converts a long to a String suitable for indexing.
    static String longToString(int64_t l);

    /// Converts a String that was returned by {@link #longToString} back to a long.
    static int64_t stringToLong(const String& str);
};

}

#endif
