/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/CachingWrapperFilter.h"
#include "fnord-fts/_CachingWrapperFilter.h"
#include "fnord-fts/OpenBitSetDISI.h"
#include "fnord-fts/IndexReader.h"

namespace Lucene {

CachingWrapperFilter::CachingWrapperFilter(const FilterPtr& filter, DeletesMode deletesMode) {
    this->filter = filter;
    this->cache = newLucene<FilterCacheDocIdSet>(deletesMode);
    this->hitCount = 0;
    this->missCount = 0;
}

CachingWrapperFilter::~CachingWrapperFilter() {
}

DocIdSetPtr CachingWrapperFilter::docIdSetToCache(const DocIdSetPtr& docIdSet, const IndexReaderPtr& reader) {
    if (!docIdSet) {
        // this is better than returning null, as the nonnull result can be cached
        return DocIdSet::EMPTY_DOCIDSET();
    } else if (docIdSet->isCacheable()) {
        return docIdSet;
    } else {
        DocIdSetIteratorPtr it(docIdSet->iterator());
        // null is allowed to be returned by iterator(), in this case we wrap with the empty set,
        // which is cacheable.
        return !it ? DocIdSet::EMPTY_DOCIDSET() : newLucene<OpenBitSetDISI>(it, reader->maxDoc());
    }
}

DocIdSetPtr CachingWrapperFilter::getDocIdSet(const IndexReaderPtr& reader) {
    LuceneObjectPtr coreKey = reader->getFieldCacheKey();
    LuceneObjectPtr delCoreKey = reader->hasDeletions() ? reader->getDeletesCacheKey() : coreKey;

    DocIdSetPtr docIdSet(boost::dynamic_pointer_cast<DocIdSet>(cache->get(reader, coreKey, delCoreKey)));
    if (docIdSet) {
        ++hitCount;
        return docIdSet;
    }

    ++missCount;

    // cache miss
    docIdSet = docIdSetToCache(filter->getDocIdSet(reader), reader);

    if (docIdSet) {
        cache->put(coreKey, delCoreKey, docIdSet);
    }

    return docIdSet;
}

String CachingWrapperFilter::toString() {
    return L"CachingWrapperFilter(" + filter->toString() + L")";
}

bool CachingWrapperFilter::equals(const LuceneObjectPtr& other) {
    if (Filter::equals(other)) {
        return true;
    }

    CachingWrapperFilterPtr otherCachingWrapperFilter(boost::dynamic_pointer_cast<CachingWrapperFilter>(other));
    if (!otherCachingWrapperFilter) {
        return false;
    }

    return this->filter->equals(otherCachingWrapperFilter->filter);
}

int32_t CachingWrapperFilter::hashCode() {
    return filter->hashCode() ^ 0x1117bf25;
}

FilterCache::FilterCache(CachingWrapperFilter::DeletesMode deletesMode) {
    this->deletesMode = deletesMode;
}

FilterCache::~FilterCache() {
}

LuceneObjectPtr FilterCache::get(const IndexReaderPtr& reader, const LuceneObjectPtr& coreKey, const LuceneObjectPtr& delCoreKey) {
    SyncLock syncLock(this);

    if (!cache) {
        cache = WeakMapObjectObject::newInstance();
    }

    LuceneObjectPtr value;
    if (deletesMode == CachingWrapperFilter::DELETES_IGNORE) {
        // key on core
        value = cache.get(coreKey);
    } else if (deletesMode == CachingWrapperFilter::DELETES_RECACHE) {
        // key on deletes, if any, else core
        value = cache.get(delCoreKey);
    } else {
        BOOST_ASSERT(deletesMode == CachingWrapperFilter::DELETES_DYNAMIC);

        // first try for exact match
        value = cache.get(delCoreKey);

        if (!value) {
            // now for core match, but dynamically AND NOT deletions
            value = cache.get(coreKey);
            if (value && reader->hasDeletions()) {
                value = mergeDeletes(reader, value);
            }
        }
    }

    return value;
}

void FilterCache::put(const LuceneObjectPtr& coreKey, const LuceneObjectPtr& delCoreKey, const LuceneObjectPtr& value) {
    SyncLock syncLock(this);

    if (deletesMode == CachingWrapperFilter::DELETES_IGNORE) {
        cache.put(coreKey, value);
    } else if (deletesMode == CachingWrapperFilter::DELETES_RECACHE) {
        cache.put(delCoreKey, value);
    } else {
        cache.put(coreKey, value);
        cache.put(delCoreKey, value);
    }
}

FilterCacheDocIdSet::FilterCacheDocIdSet(CachingWrapperFilter::DeletesMode deletesMode) : FilterCache(deletesMode) {
}

FilterCacheDocIdSet::~FilterCacheDocIdSet() {
}

LuceneObjectPtr FilterCacheDocIdSet::mergeDeletes(const IndexReaderPtr& reader, const LuceneObjectPtr& value) {
    return newLucene<FilteredCacheDocIdSet>(reader, boost::dynamic_pointer_cast<DocIdSet>(value));
}

FilteredCacheDocIdSet::FilteredCacheDocIdSet(const IndexReaderPtr& reader, const DocIdSetPtr& innerSet) : FilteredDocIdSet(innerSet) {
    this->reader = reader;
}

FilteredCacheDocIdSet::~FilteredCacheDocIdSet() {
}

bool FilteredCacheDocIdSet::match(int32_t docid) {
    return !reader->isDeleted(docid);
}

}
