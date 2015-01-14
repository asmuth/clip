/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANFILTER_H
#define SPANFILTER_H

#include "fnord-fts/search/Filter.h"

namespace fnord {
namespace fts {

/// Abstract base class providing a mechanism to restrict searches to a subset of an index and also maintains
/// and returns position information.
///
/// This is useful if you want to compare the positions from a SpanQuery with the positions of items in a filter.
/// For instance, if you had a SpanFilter that marked all the occurrences of the word "foo" in documents, and
/// then you entered a new SpanQuery containing bar, you could not only filter by the word foo, but you could
/// then compare position information for post processing.
class SpanFilter : public Filter {
public:
    virtual ~SpanFilter();
    LUCENE_CLASS(SpanFilter);

public:
    /// Returns a SpanFilterResult with true for documents which should be permitted in search results, and
    /// false for those that should not and Spans for where the true docs match.
    /// @param reader The {@link IndexReader} to load position and DocIdSet information from
    /// @return A {@link SpanFilterResult}
    virtual SpanFilterResultPtr bitSpans(const IndexReaderPtr& reader) = 0;
};

}

}
#endif
