/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMERICRANGEQUERY_H
#define NUMERICRANGEQUERY_H

#include "MultiTermQuery.h"
#include "FilteredTermEnum.h"
#include "NumericUtils.h"

namespace Lucene {

/// A {@link Query} that matches numeric values within a specified range.  To use this, you must first
/// index the numeric values using {@link NumericField} (expert: {@link NumericTokenStream}).  If your
/// terms are instead textual, you should use {@link TermRangeQuery}.  {@link NumericRangeFilter} is the
/// filter equivalent of this query.
///
/// You create a new NumericRangeQuery with the static factory methods, eg:
/// <pre>
/// QueryPtr q = NumericRangeQuery::newDoubleRange("weight", 0.3, 0.10, true, true);
/// </pre>
/// matches all documents whose double valued "weight" field ranges from 0.3 to 0.10, inclusive.
///
/// The performance of NumericRangeQuery is much better than the corresponding {@link TermRangeQuery}
/// because the number of terms that must be searched is usually far fewer, thanks to trie indexing,
/// described below.
///
/// You can optionally specify a precisionStep when creating this query.  This is necessary if you've
/// changed this configuration from its default (4) during indexing.  Lower values consume more disk
/// space but speed up searching.  Suitable values are between 1 and 8. A good starting point to test
/// is 4, which is the default value for all Numeric* classes.  See below for details.
///
/// This query defaults to {@linkplain MultiTermQuery#CONSTANT_SCORE_AUTO_REWRITE_DEFAULT} for 32 bit
/// integer ranges with precisionStep <=8 and 64 bit (long/double) ranges with precisionStep <=6.
/// Otherwise it uses {@linkplain MultiTermQuery#CONSTANT_SCORE_FILTER_REWRITE} as the number of terms
/// is likely to be high.  With precision steps of <=4, this query can be run with one of the BooleanQuery
/// rewrite methods without changing BooleanQuery's default max clause count.
///
/// How it works
///
/// See the publication about <a target="_blank" href="http://www.panfmp.org">panFMP</a>, where this
/// algorithm was described (referred to as TrieRangeQuery):
/// <blockquote><strong>Schindler, U, Diepenbroek, M</strong>, 2008.
/// Generic XML-based Framework for Metadata Portals.
/// Computers &amp; Geosciences 34 (12), 1947-1955.
/// <a href="http://dx.doi.org/10.1016/j.cageo.2008.02.023"
/// target="_blank">doi:10.1016/j.cageo.2008.02.023</a></blockquote>
///
/// A quote from this paper: Because Apache Lucene is a full-text search engine and not a conventional
/// database, it cannot handle numerical ranges (eg., field value is inside user defined bounds, even
/// dates are numerical values).  We have developed an extension to Apache Lucene that stores the
/// numerical values in a special string-encoded format with variable precision (all numerical values like
/// doubles, longs, and ints are converted to lexicographic sortable string representations and stored
/// with different precisions (for a more detailed description of how the values are stored, see {@link
/// NumericUtils}).  A range is then divided recursively into multiple intervals for searching:
/// The center of the range is searched only with the lowest possible precision in the trie, while the
/// boundaries are matched more exactly. This reduces the number of terms dramatically.
///
/// For the variant that stores long values in 8 different precisions (each reduced by 8 bits) that uses a
/// lowest precision of 1 byte, the index contains only a maximum of 256 distinct values in the lowest
/// precision.  Overall, a range could consist of a theoretical maximum of 7*255*2 + 255 = 3825 distinct
/// terms (when there is a term for every distinct value of an 8-byte-number in the index and the range
/// covers almost all of them; a maximum of 255 distinct values is used because it would always be possible
/// to reduce the full 256 values to one term with degraded precision).  In practice, we have seen up to
/// 300 terms in most cases (index with 500,000 metadata records and a uniform value distribution).
///
/// Precision Step:
/// You can choose any precisionStep when encoding values.  Lower step values mean more precisions and so
/// more terms in index (and index gets larger).  On the other hand, the maximum number of terms to match
/// reduces, which optimized query speed.  The formula to calculate the maximum term count is:
/// <pre>
/// n = [ (bitsPerValue/precisionStep - 1) * (2 ^ precisionStep - 1 ) * 2 ] + (2 ^ precisionStep - 1 )
/// </pre>
///
/// (this formula is only correct, when bitsPerValue/precisionStep is an integer; in other cases, the value
/// must be rounded up and the last summand must contain the modulo of the division as precision step).
/// For longs stored using a precision step of 4, n = 15*15*2 + 15 = 465, and for a precision step of 2,
/// n = 31*3*2 + 3 = 189. But the faster search speed is reduced by more seeking in the term enum of the
/// index.  Because of this, the ideal precisionStep value can only be found out by testing. Important: You
/// can index with a lower precision step value and test search speed using a multiple of the original step
/// value.
///
/// Good values for precisionStep are depending on usage and data type:
/// <ul>
/// <li>The default for all data types is 4, which is used, when no precisionStep is given.
/// <li>Ideal value in most cases for 64 bit data types (long, double) is 6 or 8.
/// <li>Ideal value in most cases for 32 bit data types (int) is 4.
/// <li>For low cardinality fields larger precision steps are good. If the cardinality is < 100, it is
/// fair to use {@link Integer#MAX_VALUE} (see below).
/// <li>Steps >=64 for long/double and >=32 for int/float produces one token per value in the index and
/// querying is as slow as a conventional {@link TermRangeQuery}.  But it can be used to produce fields,
/// that are solely used for sorting (in this case simply use {@link Integer#MAX_VALUE} as precisionStep).
/// Using {@link NumericField NumericFields} for sorting is ideal, because building the field cache is much
/// faster than with text-only numbers.  These fields have one term per value and therefore also work with
/// term enumeration for building distinct lists (eg. facets / preselected values to search for).
/// Sorting is also possible with range query optimized fields using one of the above precisionSteps.
/// </ul>
///
/// Comparisons of the different types of RangeQueries on an index with about 500,000 docs showed that
/// {@link TermRangeQuery} in boolean rewrite mode (with raised {@link BooleanQuery} clause count) took
/// about 30-40 secs to complete, {@link TermRangeQuery} in constant score filter rewrite mode took 5 secs
/// and executing this class took <100ms to complete (on an Opteron64 machine, 8 bit precision step). This
/// query type was developed for a geographic portal, where the performance for eg. bounding boxes or exact
/// date/time stamps is important.
class NumericRangeQuery : public MultiTermQuery {
public:
    NumericRangeQuery(const String& field, int32_t precisionStep, int32_t valSize, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive);
    virtual ~NumericRangeQuery();

    LUCENE_CLASS(NumericRangeQuery);

INTERNAL:
    String field;
    int32_t precisionStep;
    int32_t valSize;
    NumericValue min;
    NumericValue max;
    bool minInclusive;
    bool maxInclusive;

public:
    using MultiTermQuery::toString;

    /// Factory that creates a NumericRangeFilter, that filters a long range using the given precisionStep.
    static NumericRangeQueryPtr newLongRange(const String& field, int32_t precisionStep, int64_t min, int64_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a long range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeQueryPtr newLongRange(const String& field, int64_t min, int64_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int range using the given precisionStep.
    static NumericRangeQueryPtr newIntRange(const String& field, int32_t precisionStep, int32_t min, int32_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a int range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeQueryPtr newIntRange(const String& field, int32_t min, int32_t max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a double range using the given precisionStep.
    static NumericRangeQueryPtr newDoubleRange(const String& field, int32_t precisionStep, double min, double max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeFilter, that filters a double range using the default precisionStep
    /// {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).
    static NumericRangeQueryPtr newDoubleRange(const String& field, double min, double max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeQuery, that queries a int, long or double range using the given
    /// precisionStep.  You can have half-open ranges (which are in fact <= or >= queries) by setting the min
    /// or max value to VariantUtils::null(). By setting inclusive to false it will match all documents
    /// excluding the bounds, with inclusive on the boundaries are hits, too.
    static NumericRangeQueryPtr newNumericRange(const String& field, int32_t precisionStep, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive);

    /// Factory that creates a NumericRangeQuery, that queries a int, long or double range using the default
    /// precisionStep {@link NumericUtils#PRECISION_STEP_DEFAULT} (4).  You can have half-open ranges (which
    /// are in fact <= or >= queries) by setting the min or max value to VariantUtils::null(). By setting
    /// inclusive to false it will match all documents excluding the bounds, with inclusive on the boundaries
    /// are hits, too.
    static NumericRangeQueryPtr newNumericRange(const String& field, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive);

    /// Returns the field name for this query
    String getField();

    /// Returns true if the lower endpoint is inclusive
    bool includesMin();

    /// Returns true if the upper endpoint is inclusive
    bool includesMax();

    /// Returns the lower value of this range query
    NumericValue getMin();

    /// Returns the upper value of this range query
    NumericValue getMax();

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

protected:
    virtual FilteredTermEnumPtr getEnum(const IndexReaderPtr& reader);

    friend class NumericRangeTermEnum;
};

}

#endif
