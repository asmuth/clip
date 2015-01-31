/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/CachingSpanFilter.h"
#include "fnord-fts/search/_CachingSpanFilter.h"
#include "fnord-fts/search/SpanFilterResult.h"
#include "fnord-fts/index/IndexReader.h"

namespace fnord {
namespace fts {

CachingSpanFilter::CachingSpanFilter(const SpanFilterPtr& filter, CachingWrapperFilter::DeletesMode deletesMode) {
    this->filter = filter;
    if (deletesMode == CachingWrapperFilter::DELETES_DYNAMIC) {
        boost::throw_exception(IllegalArgumentException(L"DeletesMode::DYNAMIC is not supported"));
    }
    this->cache = newLucene<FilterCacheSpanFilterResult>(deletesMode);
    this->hitCount = 0;
    this->missCount = 0;
}

CachingSpanFilter::~CachingSpanFilter() {
}

DocIdSetPtr CachingSpanFilter::getDocIdSet(const IndexReaderPtr& reader) {
    SpanFilterResultPtr result(getCachedResult(reader));
    return result ? result->getDocIdSet() : DocIdSetPtr();
}

SpanFilterResultPtr CachingSpanFilter::getCachedResult(const IndexReaderPtr& reader) {
    LuceneObjectPtr coreKey = reader->getFieldCacheKey();
    LuceneObjectPtr delCoreKey = reader->hasDeletions() ? reader->getDeletesCacheKey() : coreKey;

    SpanFilterResultPtr result(std::dynamic_pointer_cast<SpanFilterResult>(cache->get(reader, coreKey, delCoreKey)));
    if (result) {
        ++hitCount;
        return result;
    }

    ++missCount;
    result = filter->bitSpans(reader);

    cache->put(coreKey, delCoreKey, result);

    return result;
}

SpanFilterResultPtr CachingSpanFilter::bitSpans(const IndexReaderPtr& reader) {
    return getCachedResult(reader);
}

String CachingSpanFilter::toString() {
    return L"CachingSpanFilter(" + filter->toString() + L")";
}

bool CachingSpanFilter::equals(const LuceneObjectPtr& other) {
    if (SpanFilter::equals(other)) {
        return true;
    }

    CachingSpanFilterPtr otherCachingSpanFilter(std::dynamic_pointer_cast<CachingSpanFilter>(other));
    if (!otherCachingSpanFilter) {
        return false;
    }

    return this->filter->equals(otherCachingSpanFilter->filter);
}

int32_t CachingSpanFilter::hashCode() {
    return filter->hashCode() ^ 0x1117bf25;
}

FilterCacheSpanFilterResult::FilterCacheSpanFilterResult(CachingWrapperFilter::DeletesMode deletesMode) : FilterCache(deletesMode) {
}

FilterCacheSpanFilterResult::~FilterCacheSpanFilterResult() {
}

LuceneObjectPtr FilterCacheSpanFilterResult::mergeDeletes(const IndexReaderPtr& reader, const LuceneObjectPtr& value) {
    boost::throw_exception(IllegalStateException(L"DeletesMode::DYNAMIC is not supported"));
    return LuceneObjectPtr();
}

}

}
