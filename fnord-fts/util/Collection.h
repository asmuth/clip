/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COLLECTION_H
#define COLLECTION_H

#include <vector>
#include "fnord-fts/util/LuceneSync.h"

namespace fnord {
namespace fts {

/// Utility template class to handle collections that can be safely copied and shared
template <class TYPE>
class Collection : public LuceneSync {
public:
    typedef Collection<TYPE> this_type;
    typedef std::shared_ptr<this_type> shared_ptr;
    typedef std::vector<TYPE> collection_type;
    typedef typename collection_type::iterator iterator;
    typedef typename collection_type::const_iterator const_iterator;
    typedef TYPE value_type;

    virtual ~Collection() {
    }

protected:
    std::shared_ptr<collection_type> container;

public:
    static this_type newInstance(int32_t size = 0) {
        this_type instance;
        instance.container = fnord::fts::newInstance<collection_type>(size);
        return instance;
    }

    template <class ITER>
    static this_type newInstance(ITER first, ITER last) {
        this_type instance;
        instance.container = fnord::fts::newInstance<collection_type>(first, last);
        return instance;
    }

    void reset() {
        resize(0);
    }

    void resize(int32_t size) {
        if (size == 0) {
            container.reset();
        } else {
            container->resize(size);
        }
    }

    int32_t size() const {
        return (int32_t)container->size();
    }

    bool empty() const {
        return container->empty();
    }

    void clear() {
        container->clear();
    }

    iterator begin() {
        return container->begin();
    }

    iterator end() {
        return container->end();
    }

    const_iterator begin() const {
        return container->begin();
    }

    const_iterator end() const {
        return container->end();
    }

    void add(const TYPE& type) {
        container->push_back(type);
    }

    void add(int32_t pos, const TYPE& type) {
        container->insert(container->begin() + pos, type);
    }

    template <class ITER>
    void addAll(ITER first, ITER last) {
        container->insert(container->end(), first, last);
    }

    template <class ITER>
    void insert(ITER pos, const TYPE& type) {
        container->insert(pos, type);
    }

    template <class ITER>
    ITER remove(ITER pos) {
        return container->erase(pos);
    }

    template <class ITER>
    ITER remove(ITER first, ITER last) {
        return container->erase(first, last);
    }

    void remove(const TYPE& type) {
        container->erase(std::remove(container->begin(), container->end(), type), container->end());
    }

    template <class PRED>
    void remove_if(PRED comp) {
        container->erase(std::remove_if(container->begin(), container->end(), comp), container->end());
    }

    TYPE removeFirst() {
        TYPE front = container->front();
        container->erase(container->begin());
        return front;
    }

    TYPE removeLast() {
        TYPE back = container->back();
        container->pop_back();
        return back;
    }

    iterator find(const TYPE& type) {
        return std::find(container->begin(), container->end(), type);
    }

    template <class PRED>
    iterator find_if(PRED comp) {
        return std::find_if(container->begin(), container->end(), comp);
    }

    bool contains(const TYPE& type) const {
        return (std::find(container->begin(), container->end(), type) != container->end());
    }

    template <class PRED>
    bool contains_if(PRED comp) const {
        return (std::find_if(container->begin(), container->end(), comp) != container->end());
    }

    bool equals(const this_type& other) const {
        return equals(other, std::equal_to<TYPE>());
    }

    template <class PRED>
    bool equals(const this_type& other, PRED comp) const {
        if (container->size() != other.container->size()) {
            return false;
        }
        return std::equal(container->begin(), container->end(), other.container->begin(), comp);
    }

    int32_t hashCode() {
        return (int32_t)(int64_t)container.get();
    }

    void swap(this_type& other) {
        container.swap(other->container);
    }

    TYPE& operator[] (int32_t pos) {
        return (*container)[pos];
    }

    const TYPE& operator[] (int32_t pos) const {
        return (*container)[pos];
    }

    operator bool() const {
        return container.get() != NULL;
    }

    bool operator! () const {
        return !container;
    }

    bool operator== (const this_type& other) {
        return (container == other.container);
    }

    bool operator!= (const this_type& other) {
        return (container != other.container);
    }
};

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1) {
    Collection<TYPE> result = Collection<TYPE>::newInstance();
    result.add(a1);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2) {
    Collection<TYPE> result = newCollection(a1);
    result.add(a2);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3) {
    Collection<TYPE> result = newCollection(a1, a2);
    result.add(a3);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4) {
    Collection<TYPE> result = newCollection(a1, a2, a3);
    result.add(a4);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4);
    result.add(a5);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5, const TYPE& a6) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4, a5);
    result.add(a6);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5, const TYPE& a6, const TYPE& a7) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4, a5, a6);
    result.add(a7);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5, const TYPE& a6, const TYPE& a7, const TYPE& a8) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4, a5, a6, a7);
    result.add(a8);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5, const TYPE& a6, const TYPE& a7, const TYPE& a8, const TYPE& a9) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4, a5, a6, a7, a8);
    result.add(a9);
    return result;
}

template <typename TYPE>
Collection<TYPE> newCollection(const TYPE& a1, const TYPE& a2, const TYPE& a3, const TYPE& a4, const TYPE& a5, const TYPE& a6, const TYPE& a7, const TYPE& a8, const TYPE& a9, const TYPE& a10) {
    Collection<TYPE> result = newCollection(a1, a2, a3, a4, a5, a6, a7, a8, a9);
    result.add(a10);
    return result;
}

}

}
#endif
