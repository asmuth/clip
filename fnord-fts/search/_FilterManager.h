/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILTERMANAGER_H
#define _FILTERMANAGER_H

#include "LuceneThread.h"

namespace Lucene {

/// Holds the filter and the last time the filter was used, to make LRU-based cache cleaning possible.
class FilterItem : public LuceneObject {
public:
    FilterItem(const FilterPtr& filter);
    virtual ~FilterItem();

    LUCENE_CLASS(FilterItem);

public:
    FilterPtr filter;
    int64_t timestamp;
};

/// Keeps the cache from getting too big.
///
/// The SortedSet sortedFilterItems is used only to sort the items from the cache, so when it's time to clean
/// up we have the TreeSet sort the FilterItems by timestamp.
///
/// Removes 1.5 * the numbers of items to make the cache smaller.
/// For example: If cache clean size is 10, and the cache is at 15, we would remove (15 - 10) * 1.5 = 7.5
/// round up to 8.  This way we clean the cache a bit more, and avoid having the cache cleaner having to do
/// it frequently.
class FilterCleaner : public LuceneThread {
public:
    FilterCleaner(const FilterManagerPtr& manager);
    virtual ~FilterCleaner();

    LUCENE_CLASS(FilterCleaner);

protected:
    FilterManagerWeakPtr _manager;
    bool running;
    MapLongInt sortedFilterItems;

public:
    virtual void run();
};

}

#endif
