/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Filter caching singleton.  It can be used to save filters locally for reuse.  Also could be used as a
/// persistent storage for any filter as long as the filter provides a proper hashCode(), as that is used
/// as the key in the cache.
///
/// The cache is periodically cleaned up from a separate thread to ensure the cache doesn't exceed the
/// maximum size.
class FilterManager : public LuceneObject {
public:
    /// Sets up the FilterManager singleton.
    FilterManager();
    virtual ~FilterManager();

    LUCENE_CLASS(FilterManager);

protected:
    /// The default maximum number of Filters in the cache
    static const int32_t DEFAULT_CACHE_CLEAN_SIZE;

    /// The default frequency of cache cleanup
    static const int64_t DEFAULT_CACHE_SLEEP_TIME;

    /// The cache itself
    MapIntFilterItem cache;

    /// Maximum allowed cache size
    int32_t cacheCleanSize;

    /// Cache cleaning frequency
    int64_t cleanSleepTime;

    /// Cache cleaner that runs in a separate thread
    FilterCleanerPtr filterCleaner;

public:
    virtual void initialize();

    static FilterManagerPtr getInstance();

    /// Sets the max size that cache should reach before it is cleaned up
    /// @param cacheCleanSize maximum allowed cache size
    void setCacheSize(int32_t cacheCleanSize);

    /// Sets the cache cleaning frequency in milliseconds.
    /// @param cleanSleepTime cleaning frequency in milliseconds
    void setCleanThreadSleepTime(int64_t cleanSleepTime);

    /// Returns the cached version of the filter.  Allows the caller to pass up a small filter but this will
    /// keep a persistent version around and allow the caching filter to do its job.
    /// @param filter The input filter
    /// @return The cached version of the filter
    FilterPtr getFilter(const FilterPtr& filter);

    friend class FilterCleaner;
};

}

}
#endif
