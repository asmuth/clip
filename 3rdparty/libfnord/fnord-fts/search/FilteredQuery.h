/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTEREDQUERY_H
#define FILTEREDQUERY_H

#include "fnord-fts/search/Query.h"

namespace fnord {
namespace fts {

/// A query that applies a filter to the results of another query.
///
/// Note: the bits are retrieved from the filter each time this query is used in a search - use a
/// CachingWrapperFilter to avoid regenerating the bits every time.
///
/// @see CachingWrapperFilter
class FilteredQuery : public Query {
public:
    /// Constructs a new query which applies a filter to the results of the original query.
    /// Filter::getDocIdSet() will be called every time this query is used in a search.
    /// @param query Query to be filtered, cannot be null.
    /// @param filter Filter to apply to query results, cannot be null.
    FilteredQuery(const QueryPtr& query, const FilterPtr& filter);

    virtual ~FilteredQuery();

    LUCENE_CLASS(FilteredQuery);

private:
    QueryPtr query;
    FilterPtr filter;

public:
    using Query::toString;

    /// Returns a Weight that applies the filter to the enclosed query's Weight.
    /// This is accomplished by overriding the Scorer returned by the Weight.
    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    /// Rewrites the wrapped query.
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    QueryPtr getQuery();
    FilterPtr getFilter();

    virtual void extractTerms(SetTerm terms);

    /// Prints a user-readable version of this query.
    virtual String toString(const String& field);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    friend class FilteredQueryWeight;
};

}

}
#endif
