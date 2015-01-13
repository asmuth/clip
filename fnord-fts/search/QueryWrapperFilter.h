/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYWRAPPERFILTER_H
#define QUERYWRAPPERFILTER_H

#include "fnord-fts/search/Filter.h"

namespace Lucene {

/// Constrains search results to only match those which also match a provided query.
///
/// This could be used, for example, with a {@link TermRangeQuery} on a suitably formatted date field to
/// implement date filtering.  One could re-use a single QueryFilter that matches, eg., only documents
/// modified within the last week.  The QueryFilter and TermRangeQuery would only need to be reconstructed
/// once per day.
class QueryWrapperFilter : public Filter {
public:
    /// Constructs a filter which only matches documents matching query.
    QueryWrapperFilter(const QueryPtr& query);

    virtual ~QueryWrapperFilter();

    LUCENE_CLASS(QueryWrapperFilter);

protected:
    QueryPtr query;

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

#endif
