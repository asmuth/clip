/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMERICRANGEFILTER_H
#define NUMERICRANGEFILTER_H

#include "MultiTermQueryWrapperFilter.h"

namespace Lucene {

/// A {@link Filter} that only accepts numeric values within a specified range. To use this, you must first
/// index the numeric values using {@link NumericField} ({@link NumericTokenStream}).
///
/// You create a new NumericRangeFilter with the static factory methods, eg:
/// <pre>
/// FilterPtr f = NumericRangeFilter::newDoubleRange(L"weight", 0.3, 0.10, true, true);
/// </pre>
/// accepts all documents whose double valued "weight" field ranges from 0.3 to 0.10, inclusive.
///
/// See {@link NumericRangeQuery} for details on how Lucene indexes and searches numeric valued fields.
class NumericRangeFilter : public MultiTermQueryWrapperFilter {
public:
    NumericRangeFilter(const NumericRangeQueryPtr& query);
    virtual ~NumericRangeFilter();

    LUCENE_CLASS(NumericRangeFilter);

public:
    /// Factory that creates a NumericRangeFilter, that filters a long range using the given precisionStep.
    static NumericRangeFilterPtr newLongRange(const String& field, int32_t precisionStep, int64_t min, int64_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a long range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeFilterPtr newLongRange(const String& field, int64_t min, int64_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int range using the given precisionStep.
    static NumericRangeFilterPtr newIntRange(const String& field, int32_t precisionStep, int32_t min, int32_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeFilterPtr newIntRange(const String& field, int32_t min, int32_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a double range using the given precisionStep.
    static NumericRangeFilterPtr newDoubleRange(const String& field, int32_t precisionStep, double min, double max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a double range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeFilterPtr newDoubleRange(const String& field, double min, double max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int, long or double range using the given
    /// precisionStep.  You can have half-open ranges (which are in fact <= or >= queries) by setting the min
    /// or max value to VariantUtils::null(). By setting inclusive to false it will match all documents
    /// excluding the bounds, with inclusive on the boundaries are hits, too.
    static NumericRangeFilterPtr newNumericRange(const String& field, int32_t precisionStep, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int, long or double range range using the default
    /// precisionStep {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).  You can have half-open ranges (which are in
    /// fact <= or >= queries) by setting the min or max value to VariantUtils::null(). By setting inclusive to false
    /// it will match all documents excluding the bounds, with inclusive on the boundaries are hits, too.
    static NumericRangeFilterPtr newNumericRange(const String& field, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive);

    /// Returns the field name for this filter
    String getField();

    /// Returns true if the lower endpoint is inclusive
    bool includesMin();

    /// Returns true if the upper endpoint is inclusive
    bool includesMax();

    /// Returns the lower value of this range filter
    NumericValue getMin();

    /// Returns the upper value of this range filter
    NumericValue getMax();
};

}

#endif
