/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/FilterManager.h"
#include "fnord-fts/search/_FilterManager.h"
#include "fnord-fts/search/Filter.h"
#include "fnord-fts/util/MiscUtils.h"

namespace Lucene {

/// The default maximum number of Filters in the cache
const int32_t FilterManager::DEFAULT_CACHE_CLEAN_SIZE = 100;

/// The default frequency of cache cleanup
const int64_t FilterManager::DEFAULT_CACHE_SLEEP_TIME = 1000 * 60 * 10;

FilterManager::FilterManager() {
}

FilterManager::~FilterManager() {
}

void FilterManager::initialize() {
    cache = MapIntFilterItem::newInstance();
    cacheCleanSize = DEFAULT_CACHE_CLEAN_SIZE; // Let the cache get to 100 items
    cleanSleepTime = DEFAULT_CACHE_SLEEP_TIME; // 10 minutes between cleanings

    filterCleaner = newLucene<FilterCleaner>(shared_from_this());
    filterCleaner->start();
}

FilterManagerPtr FilterManager::getInstance() {
    static FilterManagerPtr manager;
    if (!manager) {
        manager = newLucene<FilterManager>();
        CycleCheck::addStatic(manager);
    }
    return manager;
}

void FilterManager::setCacheSize(int32_t cacheCleanSize) {
    this->cacheCleanSize = cacheCleanSize;
}

void FilterManager::setCleanThreadSleepTime(int64_t cleanSleepTime) {
    this->cleanSleepTime = cleanSleepTime;
}

FilterPtr FilterManager::getFilter(const FilterPtr& filter) {
    SyncLock parentLock(&cache);
    FilterItemPtr fi(cache.get(filter->hashCode()));
    if (fi) {
        fi->timestamp = MiscUtils::currentTimeMillis();
        return fi->filter;
    }
    cache.put(filter->hashCode(), newLucene<FilterItem>(filter));
    return filter;
}

FilterItem::FilterItem(const FilterPtr& filter) {
    this->filter = filter;
    this->timestamp = MiscUtils::currentTimeMillis();
}

FilterItem::~FilterItem() {
}

FilterCleaner::FilterCleaner(const FilterManagerPtr& manager) {
    _manager = manager;
    running = true;
}

FilterCleaner::~FilterCleaner() {
}

void FilterCleaner::run() {
    while (running) {
        FilterManagerPtr manager(_manager);

        // sort items from oldest to newest we delete the oldest filters
        if (manager->cache.size() > manager->cacheCleanSize) {
            // empty the temporary set
            sortedFilterItems.clear();

            {
                SyncLock parentLock(&manager->cache);
                for (MapIntFilterItem::iterator item = manager->cache.begin(); item != manager->cache.end(); ++item) {
                    sortedFilterItems.put(item->second->timestamp, item->first);
                }
                int32_t numToDelete = (int32_t)((double)(sortedFilterItems.size() - manager->cacheCleanSize) * 1.5);
                int32_t counter = 0;
                // loop over the set and delete all of the cache entries not used in a while
                for (MapLongInt::iterator item = sortedFilterItems.begin(); item != sortedFilterItems.end() && counter++ < numToDelete; ++item) {
                    manager->cache.remove(item->second);
                }
            }

            // empty the set so we don't tie up the memory
            sortedFilterItems.clear();
        }

        // take a nap
        LuceneThread::threadSleep(manager->cleanSleepTime);
    }
}

}
