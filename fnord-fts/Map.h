/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MAP_H
#define MAP_H

#include <map>
#include "LuceneSync.h"

namespace Lucene {

/// Utility template class to handle maps that can be safely copied and shared
template < class KEY, class VALUE, class LESS = std::less<KEY> >
class Map : public LuceneSync {
public:
    typedef Map<KEY, VALUE, LESS> this_type;
    typedef std::pair<KEY, VALUE> key_value;
    typedef std::map<KEY, VALUE, LESS> map_type;
    typedef typename map_type::iterator iterator;
    typedef typename map_type::const_iterator const_iterator;
    typedef KEY key_type;
    typedef VALUE value_type;

    virtual ~Map() {
    }

protected:
    std::shared_ptr<map_type> mapContainer;

public:
    static this_type newInstance() {
        this_type instance;
        instance.mapContainer = Lucene::newInstance<map_type>();
        return instance;
    }

    void reset() {
        mapContainer.reset();
    }

    int32_t size() const {
        return (int32_t)mapContainer->size();
    }

    bool empty() const {
        return mapContainer->empty();
    }

    void clear() {
        mapContainer->clear();
    }

    iterator begin() {
        return mapContainer->begin();
    }

    iterator end() {
        return mapContainer->end();
    }

    const_iterator begin() const {
        return mapContainer->begin();
    }

    const_iterator end() const {
        return mapContainer->end();
    }

    operator bool() const {
        return mapContainer.get() != NULL;
    }

    bool operator! () const {
        return !mapContainer;
    }

    map_type& operator= (const map_type& other) {
        mapContainer = other.mapContainer;
        return *this;
    }

    void put(const KEY& key, const VALUE& value) {
        (*mapContainer)[key] = value;
    }

    template <class ITER>
    void putAll(ITER first, ITER last) {
        for (iterator current = first; current != last; ++current) {
            (*mapContainer)[current->first] = current->second;
        }
    }

    template <class ITER>
    void remove(ITER pos) {
        mapContainer->erase(pos);
    }

    template <class ITER>
    ITER remove(ITER first, ITER last) {
        return mapContainer->erase(first, last);
    }

    bool remove(const KEY& key) {
        return (mapContainer->erase(key) > 0);
    }

    iterator find(const KEY& key) {
        return mapContainer->find(key);
    }

    VALUE get(const KEY& key) const {
        iterator findValue = mapContainer->find(key);
        return findValue == mapContainer->end() ? VALUE() : findValue->second;
    }

    bool contains(const KEY& key) const {
        return (mapContainer->find(key) != mapContainer->end());
    }

    VALUE& operator[] (const KEY& key) {
        return (*mapContainer)[key];
    }
};

}

#endif
