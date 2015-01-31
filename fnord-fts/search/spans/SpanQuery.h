/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANQUERY_H
#define SPANQUERY_H

#include "fnord-fts/search/Query.h"

namespace fnord {
namespace fts {

/// Base class for span-based queries.
class SpanQuery : public Query {
public:
    virtual ~SpanQuery();
    LUCENE_CLASS(SpanQuery);

public:
    /// Returns the matches for this query in an index.  Used internally to search for spans.
    virtual SpansPtr getSpans(const IndexReaderPtr& reader) = 0;

    /// Returns the name of the field matched by this query.
    virtual String getField() = 0;

    virtual WeightPtr createWeight(const SearcherPtr& searcher);
};

}

}
#endif
