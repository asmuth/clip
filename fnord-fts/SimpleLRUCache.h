/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLELRUCACHE_H
#define SIMPLELRUCACHE_H

#include <list>
#include "LuceneObject.h"

namespace Lucene {

/// General purpose LRU cache map.
/// Accessing an entry will keep the entry cached.  {@link #get(const KEY&)} and
/// {@link #put(const KEY&, const VALUE&)} results in an access to the corresponding entry.
template <class KEY, class VALUE, class HASH, class EQUAL>
class SimpleLRUCache : public LuceneObject {
public:
    typedef std::pair<KEY, VALUE> key_value;
    typedef std::list< key_value > key_list;
    typedef typename key_list::const_iterator const_iterator;
    typedef boost::unordered_map<KEY, typename key_list::iterator, HASH, EQUAL> map_type;
    typedef typename map_type::const_iterator map_iterator;

    SimpleLRUCache(int32_t cacheSize) {
        this->cacheSize = cacheSize;
    }

    virtual ~SimpleLRUCache() {
    }

protected:
    int32_t cacheSize;
    key_list cacheList;
    map_type cacheMap;

public:
    void put(const KEY& key, const VALUE& value) {
        cacheList.push_front(std::make_pair(key, value));
        cacheMap[key] = cacheList.begin();

        if ((int32_t)cacheList.size() > cacheSize) {
            cacheMap.erase(cacheList.back().first);
            cacheList.pop_back();
        }
    }

    VALUE get(const KEY& key) {
        map_iterator find = cacheMap.find(key);
        if (find == cacheMap.end()) {
            return VALUE();
        }

        VALUE value(find->second->second);
        cacheList.erase(find->second);
        cacheList.push_front(std::make_pair(key, value));
        cacheMap[key] = cacheList.begin();

        return value;
    }

    bool contains(const KEY& key) const {
        return (cacheMap.find(key) != cacheMap.end());
    }

    int32_t size() const {
        return (int32_t)cacheList.size();
    }

    const_iterator begin() const {
        return cacheList.begin();
    }

    const_iterator end() const {
        return cacheList.end();
    }
};

};

#endif
