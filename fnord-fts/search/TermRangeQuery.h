/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMRANGEQUERY_H
#define TERMRANGEQUERY_H

#include "MultiTermQuery.h"

namespace Lucene {

/// A Query that matches documents within an range of terms.
///
/// This query matches the documents looking for terms that fall into the supplied range according to {@link
/// String#compare(String)}, unless a Collator is provided. It is not intended for numerical ranges; use {@link
/// NumericRangeQuery} instead.
///
/// This query uses the {@link MultiTermQuery#CONSTANT_SCORE_AUTO_REWRITE_DEFAULT} rewrite method.
class TermRangeQuery : public MultiTermQuery {
public:
    /// Constructs a query selecting all terms greater/equal than lowerTerm but less/equal than upperTerm.
    ///
    /// If an endpoint is null, it is said to be "open". Either or both endpoints may be open.  Open endpoints
    /// may not be exclusive (you can't select all but the first or last term without explicitly specifying the
    /// term to exclude.)
    ///
    /// If collator is not null, it will be used to decide whether index terms are within the given range, rather
    /// than using the Unicode code point order in which index terms are stored.
    ///
    /// Warning: Using this constructor and supplying a non-null value in the collator parameter will cause every
    /// single index Term in the Field referenced by lowerTerm and/or upperTerm to be examined.  Depending on the
    /// number of index Terms in this Field, the operation could be very slow.
    ///
    /// @param lowerTerm The Term text at the lower end of the range
    /// @param upperTerm The Term text at the upper end of the range
    /// @param includeLower If true, the lowerTerm is included in the range.
    /// @param includeUpper If true, the upperTerm is included in the range.
    /// @param collator The collator to use to collate index Terms, to determine their membership in the range
    /// bounded by lowerTerm and upperTerm.
    TermRangeQuery(const String& fieldName, StringValue lowerTerm, StringValue upperTerm, bool includeLower,
                   bool includeUpper, CollatorPtr collator = CollatorPtr());

    virtual ~TermRangeQuery();

    LUCENE_CLASS(TermRangeQuery);

protected:
    StringValue lowerTerm;
    StringValue upperTerm;
    CollatorPtr collator;
    String field;
    bool includeLower;
    bool includeUpper;

public:
    using MultiTermQuery::toString;

    /// Returns the field name for this query
    String getField();

    /// Returns the lower value of this range query
    String getLowerTerm();

    /// Returns the upper value of this range query
    String getUpperTerm();

    /// Returns true if the lower endpoint is inclusive
    bool includesLower();

    /// Returns true if the upper endpoint is inclusive
    bool includesUpper();

    /// Returns the collator used to determine range inclusion, if any.
    CollatorPtr getCollator();

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual String toString(const String& field);
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);

protected:
    virtual FilteredTermEnumPtr getEnum(const IndexReaderPtr& reader);
};

}

#endif
