/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CACHINGWRAPPERFILTER_H
#define _CACHINGWRAPPERFILTER_H

#include "FilteredDocIdSet.h"

namespace Lucene {

class FilterCache : public LuceneObject {
public:
    FilterCache(CachingWrapperFilter::DeletesMode deletesMode);
    virtual ~FilterCache();

    LUCENE_CLASS(FilterCache);

public:
    WeakMapObjectObject cache;
    CachingWrapperFilter::DeletesMode deletesMode;

public:
    virtual LuceneObjectPtr get(const IndexReaderPtr& reader, const LuceneObjectPtr& coreKey, const LuceneObjectPtr& delCoreKey);
    virtual void put(const LuceneObjectPtr& coreKey, const LuceneObjectPtr& delCoreKey, const LuceneObjectPtr& value);

protected:
    virtual LuceneObjectPtr mergeDeletes(const IndexReaderPtr& reader, const LuceneObjectPtr& value) = 0;
};

class FilterCacheDocIdSet : public FilterCache {
public:
    FilterCacheDocIdSet(CachingWrapperFilter::DeletesMode deletesMode);
    virtual ~FilterCacheDocIdSet();

    LUCENE_CLASS(FilterCacheDocIdSet);

protected:
    virtual LuceneObjectPtr mergeDeletes(const IndexReaderPtr& reader, const LuceneObjectPtr& value);
};

class FilteredCacheDocIdSet : public FilteredDocIdSet {
public:
    FilteredCacheDocIdSet(const IndexReaderPtr& reader, const DocIdSetPtr& innerSet);
    virtual ~FilteredCacheDocIdSet();

    LUCENE_CLASS(FilteredCacheDocIdSet);

protected:
    IndexReaderPtr reader;

protected:
    virtual bool match(int32_t docid);
};

}

#endif
