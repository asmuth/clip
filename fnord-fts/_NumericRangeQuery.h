/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _NUMERICRANGEQUERY_H
#define _NUMERICRANGEQUERY_H

#include "FilteredTermEnum.h"
#include "NumericUtils.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating all terms that match the sub-ranges for trie range queries.
///
/// Warning: This term enumeration is not guaranteed to be always ordered by {@link Term#compareTo}.  The
/// ordering depends on how {@link NumericUtils#splitLongRange} and {@link NumericUtils#splitIntRange}
/// generates the sub-ranges.  For {@link MultiTermQuery} ordering is not relevant.
class NumericRangeTermEnum : public FilteredTermEnum {
public:
    NumericRangeTermEnum(const NumericRangeQueryPtr& query, const IndexReaderPtr& reader);
    virtual ~NumericRangeTermEnum();

    LUCENE_CLASS(NumericRangeTermEnum);

protected:
    NumericRangeQueryWeakPtr _query;
    IndexReaderPtr reader;
    Collection<String> rangeBounds;
    TermPtr termTemplate;
    String currentUpperBound;

public:
    virtual double difference();

    /// Increments the enumeration to the next element.  True if one exists.
    virtual bool next();

    /// Closes the enumeration to further activity, freeing resources.
    virtual void close();

protected:
    /// This is a dummy, it is not used by this class.
    virtual bool endEnum();

    /// This is a dummy, it is not used by this class.
    virtual void setEnum(const TermEnumPtr& actualEnum);

    /// Compares if current upper bound is reached, this also updates the term count for statistics.
    /// In contrast to {@link FilteredTermEnum}, a return value of false ends iterating the current enum
    /// and forwards to the next sub-range.
    virtual bool termCompare(const TermPtr& term);
};

class NumericLongRangeBuilder : public LongRangeBuilder {
public:
    NumericLongRangeBuilder(Collection<String> rangeBounds);
    virtual ~NumericLongRangeBuilder();

    LUCENE_CLASS(NumericLongRangeBuilder);

protected:
    Collection<String> rangeBounds;

public:
    virtual void addRange(const String& minPrefixCoded, const String& maxPrefixCoded);
};

class NumericIntRangeBuilder : public IntRangeBuilder {
public:
    NumericIntRangeBuilder(Collection<String> rangeBounds);
    virtual ~NumericIntRangeBuilder();

    LUCENE_CLASS(NumericIntRangeBuilder);

protected:
    Collection<String> rangeBounds;

public:
    virtual void addRange(const String& minPrefixCoded, const String& maxPrefixCoded);
};

}

#endif
