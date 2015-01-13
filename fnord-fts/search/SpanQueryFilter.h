/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANQUERYFILTER_H
#define SPANQUERYFILTER_H

#include "fnord-fts/search/SpanFilter.h"

namespace Lucene {

/// Constrains search results to only match those which also match a provided query.  Also provides position
/// information about where each document matches at the cost of extra space compared with the
/// QueryWrapperFilter.  There is an added cost to this above what is stored in a {@link QueryWrapperFilter}.
/// Namely, the position information for each matching document is stored.
///
/// This filter does not cache.  See the {@link CachingSpanFilter} for a wrapper that caches.
class SpanQueryFilter : public SpanFilter {
public:
    /// Constructs a filter which only matches documents matching query.
    /// @param query The {@link SpanQuery} to use as the basis for the Filter.
    SpanQueryFilter(const SpanQueryPtr& query = SpanQueryPtr());

    virtual ~SpanQueryFilter();

    LUCENE_CLASS(SpanQueryFilter);

protected:
    SpanQueryPtr query;

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
    virtual SpanFilterResultPtr bitSpans(const IndexReaderPtr& reader);

    SpanQueryPtr getQuery();

    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

#endif
