/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DATEFIELD_H
#define DATEFIELD_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Provides support for converting dates to strings and vice-versa.  The strings are structured so that
/// lexicographic sorting orders by date, which makes them suitable for use as field values and search terms.
///
/// Note that this class saves dates with millisecond granularity, which is bad for {@link TermRangeQuery} and
/// {@link PrefixQuery}, as those queries are expanded to a BooleanQuery with a potentially large number of terms
/// when searching. Thus you might want to use {@link DateTools} instead.
///
/// Note: dates before 1970 cannot be used, and therefore cannot be indexed when using this class. See {@link
/// DateTools} for an alternative without such a limitation.
///
/// Another approach is {@link NumericUtils}, which provides a sortable binary representation (prefix encoded)
/// of numeric values, which date/time are.  For indexing a {@link Date} or {@link Calendar}, just get the unix
/// timestamp as long using {@link Date#getTime} or {@link Calendar#getTimeInMillis} and index this as a numeric
/// value with {@link NumericField} and use {@link NumericRangeQuery} to query it.
///
/// @deprecated If you build a new index, use {@link DateTools} or {@link NumericField} instead.  This class is
/// included for use with existing indices and will be removed in a future release (possibly Lucene 4.0).
class DateField : public LuceneObject {
public:
    virtual ~DateField();

    LUCENE_CLASS(DateField);

protected:
    static int32_t DATE_LEN();

public:
    static const String& MIN_DATE_STRING();
    static const String& MAX_DATE_STRING();

    /// Converts a Date to a string suitable for indexing.
    static String dateToString(const boost::posix_time::ptime& date);

    /// Converts a millisecond time to a string suitable for indexing.
    static String timeToString(int64_t time);

    /// Converts a string-encoded date into a millisecond time.
    static int64_t stringToTime(const String& s);
};

}

}
#endif
