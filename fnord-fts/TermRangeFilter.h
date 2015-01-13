/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMRANGEFILTER_H
#define TERMRANGEFILTER_H

#include "MultiTermQueryWrapperFilter.h"

namespace Lucene {

/// A Filter that restricts search results to a range of term values in a given field.
///
/// This filter matches the documents looking for terms that fall into the supplied range according to {@link
/// String#compare(String)}, unless a Collator is provided. It is not intended for numerical ranges; use {@link
/// NumericRangeFilter} instead.
///
/// If you construct a large number of range filters with different ranges but on the same field, {@link
/// FieldCacheRangeFilter} may have significantly better performance.
class TermRangeFilter : public MultiTermQueryWrapperFilter {
public:
    /// Warning: Using this constructor and supplying a non-null value in the collator parameter will cause
    /// every single index Term in the Field referenced by lowerTerm and/or upperTerm to be examined.  Depending
    /// on the number of index Terms in this Field, the operation could be very slow.
    /// @param lowerTerm The lower bound on this range
    /// @param upperTerm The upper bound on this range
    /// @param includeLower Does this range include the lower bound?
    /// @param includeUpper Does this range include the upper bound?
    /// @param collator The collator to use when determining range inclusion; set to null to use Unicode code
    /// point ordering instead of collation.
    TermRangeFilter(const String& fieldName, StringValue lowerTerm, StringValue upperTerm, bool includeLower,
                    bool includeUpper, CollatorPtr collator = CollatorPtr());

    virtual ~TermRangeFilter();

    LUCENE_CLASS(TermRangeFilter);

public:
    /// Constructs a filter for field fieldName matching less than or equal to upperTerm.
    static TermRangeFilterPtr Less(const String& fieldName, StringValue upperTerm);

    /// Constructs a filter for field fieldName matching greater than or equal to lowerTerm.
    static TermRangeFilterPtr More(const String& fieldName, StringValue lowerTerm);

    /// Returns the field name for this filter
    String getField();

    /// Returns the lower value of this range filter
    String getLowerTerm();

    /// Returns the upper value of this range filter
    String getUpperTerm();

    /// Returns true if the lower endpoint is inclusive
    bool includesLower();

    /// Returns true if the upper endpoint is inclusive
    bool includesUpper();

    /// Returns the collator used to determine range inclusion, if any.
    CollatorPtr getCollator();
};

}

#endif
