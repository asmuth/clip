/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHERANGEFILTER_H
#define FIELDCACHERANGEFILTER_H

#include "Filter.h"
#include "fnord-fts/search/FieldCache.h"

namespace Lucene {

/// A range filter built on top of a cached single term field (in {@link FieldCache}).
///
/// FieldCacheRangeFilter builds a single cache for the field the first time it is used.  Each subsequent
/// FieldCacheRangeFilter on the same field then reuses this cache, even if the range itself changes.
///
/// This means that FieldCacheRangeFilter is much faster (sometimes more than 100x as fast) as building a
/// {@link TermRangeFilter}, if using a {@link #newStringRange}.  However, if the range never changes it is
/// slower (around 2x as slow) than building a CachingWrapperFilter on top of a single {@link TermRangeFilter}.
///
/// For numeric data types, this filter may be significantly faster than {@link NumericRangeFilter}.
/// Furthermore, it does not need the numeric values encoded by {@link NumericField}. But it has the problem
/// that it only works with exact one value/document (see below).
///
/// As with all {@link FieldCache} based functionality, FieldCacheRangeFilter is only valid for fields which
/// exact one term for each document (except for {@link #newStringRange} where 0 terms are also allowed). Due
/// to a restriction of {@link FieldCache}, for numeric ranges all terms that do not have a numeric value, 0
/// is assumed.
///
/// Thus it works on dates, prices and other single value fields but will not work on regular text fields. It
/// is preferable to use a NOT_ANALYZED field to ensure that there is only a single term.
///
/// Do not instantiate this template directly, use one of the static factory methods available, that create a
/// correct instance for different data types supported by {@link FieldCache}.
class FieldCacheRangeFilter : public Filter {
public:
    FieldCacheRangeFilter(const String& field, const ParserPtr& parser, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilter();

    LUCENE_CLASS(FieldCacheRangeFilter);

INTERNAL:
    String field;
    ParserPtr parser;
    bool includeLower;
    bool includeUpper;

public:
    /// Creates a string range filter using {@link FieldCache#getStringIndex}. This works with all fields containing
    /// zero or one term in the field. The range can be half-open by setting one of the values to null.
    static FieldCacheRangeFilterPtr newStringRange(const String& field, const String& lowerVal, const String& upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getBytes(IndexReaderPtr, String)}. This works with all
    /// byte fields containing exactly one numeric term in the field. The range can be half-open by setting one of the
    /// values to null.
    static FieldCacheRangeFilterPtr newByteRange(const String& field, uint8_t lowerVal, uint8_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getBytes(IndexReaderPtr, String, ByteParserPtr)}. This
    /// works with all byte fields containing exactly one numeric term in the field.  The range can be half-open by
    /// setting one of the values to null.
    static FieldCacheRangeFilterPtr newByteRange(const String& field, const ByteParserPtr& parser, uint8_t lowerVal, uint8_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getInts(IndexReaderPtr, String)}. This works with all
    /// int fields containing exactly one numeric term in the field. The range can be half-open by setting one of the
    /// values to null.
    static FieldCacheRangeFilterPtr newIntRange(const String& field, int32_t lowerVal, int32_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getInts(IndexReaderPtr, String, IntParserPtr)}. This
    /// works with all int fields containing exactly one numeric term in the field.  The range can be half-open by
    /// setting one of the values to null.
    static FieldCacheRangeFilterPtr newIntRange(const String& field, const IntParserPtr& parser, int32_t lowerVal, int32_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getLongs(IndexReaderPtr, String)}. This works with all
    /// long fields containing exactly one numeric term in the field. The range can be half-open by setting one of the
    /// values to null.
    static FieldCacheRangeFilterPtr newLongRange(const String& field, int64_t lowerVal, int64_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getLongs(IndexReaderPtr, String, LongParserPtr)}. This
    /// works with all long fields containing exactly one numeric term in the field.  The range can be half-open by
    /// setting one of the values to null.
    static FieldCacheRangeFilterPtr newLongRange(const String& field, const LongParserPtr& parser, int64_t lowerVal, int64_t upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getDoubles(IndexReaderPtr, String)}. This works with all
    /// long fields containing exactly one numeric term in the field. The range can be half-open by setting one of the
    /// values to null.
    static FieldCacheRangeFilterPtr newDoubleRange(const String& field, double lowerVal, double upperVal, bool includeLower, bool includeUpper);

    /// Creates a numeric range filter using {@link FieldCache#getDoubles(IndexReaderPtr, String, DoubleParserPtr)}. This
    /// works with all long fields containing exactly one numeric term in the field.  The range can be half-open by
    /// setting one of the values to null.
    static FieldCacheRangeFilterPtr newDoubleRange(const String& field, const DoubleParserPtr& parser, double lowerVal, double upperVal, bool includeLower, bool includeUpper);

    virtual String toString() = 0;
    virtual bool equals(const LuceneObjectPtr& other) = 0;
    virtual int32_t hashCode() = 0;

    /// Returns the field name for this filter
    virtual String getField();

    /// Returns true if the lower endpoint is inclusive
    virtual bool includesLower();

    /// Returns true if the upper endpoint is inclusive
    virtual bool includesUpper();

    /// Returns the current numeric parser
    virtual ParserPtr getParser();
};

}

#endif
