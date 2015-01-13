/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CACHINGSPANFILTER_H
#define _CACHINGSPANFILTER_H

#include "_CachingWrapperFilter.h"

namespace Lucene {

class FilterCacheSpanFilterResult : public FilterCache {
public:
    FilterCacheSpanFilterResult(CachingWrapperFilter::DeletesMode deletesMode);
    virtual ~FilterCacheSpanFilterResult();

    LUCENE_CLASS(FilterCacheSpanFilterResult);

protected:
    virtual LuceneObjectPtr mergeDeletes(const IndexReaderPtr& reader, const LuceneObjectPtr& value);
};

}

#endif
