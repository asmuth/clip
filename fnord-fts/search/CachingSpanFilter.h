/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CACHINGSPANFILTER_H
#define CACHINGSPANFILTER_H

#include "fnord-fts/search/SpanFilter.h"
#include "fnord-fts/search/CachingWrapperFilter.h"

namespace Lucene {

/// Wraps another SpanFilter's result and caches it.  The purpose is to allow filters to simply filter,
/// and then wrap with this class to add caching.
class CachingSpanFilter : public SpanFilter {
public:
    /// New deletions always result in a cache miss, by default ({@link CachingWrapperFilter#RECACHE}.
    CachingSpanFilter(const SpanFilterPtr& filter, CachingWrapperFilter::DeletesMode deletesMode = CachingWrapperFilter::DELETES_RECACHE);
    virtual ~CachingSpanFilter();

    LUCENE_CLASS(CachingSpanFilter);

protected:
    SpanFilterPtr filter;
    FilterCachePtr cache;

public:
    // for testing
    int32_t hitCount;
    int32_t missCount;

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
    virtual SpanFilterResultPtr bitSpans(const IndexReaderPtr& reader);

    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

protected:
    SpanFilterResultPtr getCachedResult(const IndexReaderPtr& reader);
};

}

#endif
