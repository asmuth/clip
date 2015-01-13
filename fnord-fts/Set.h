/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SET_H
#define SET_H

#include <set>
#include "LuceneSync.h"

namespace Lucene {

/// Utility template class to handle set based collections that can be safely copied and shared
template < class TYPE, class LESS = std::less<TYPE> >
class Set : public LuceneSync {
public:
    typedef Set<TYPE, LESS> this_type;
    typedef std::set<TYPE, LESS> set_type;
    typedef typename set_type::iterator iterator;
    typedef typename set_type::const_iterator const_iterator;
    typedef TYPE value_type;

    virtual ~Set() {
    }

protected:
    std::shared_ptr<set_type> setContainer;

public:
    static this_type newInstance() {
        this_type instance;
        instance.setContainer = Lucene::newInstance<set_type>();
        return instance;
    }

    template <class ITER>
    static this_type newInstance(ITER first, ITER last) {
        this_type instance;
        instance.setContainer = Lucene::newInstance<set_type>(first, last);
        return instance;
    }

    void reset() {
        setContainer.reset();
    }

    int32_t size() const {
        return (int32_t)setContainer->size();
    }

    bool empty() const {
        return setContainer->empty();
    }

    void clear() {
        setContainer->clear();
    }

    iterator begin() {
        return setContainer->begin();
    }

    iterator end() {
        return setContainer->end();
    }

    const_iterator begin() const {
        return setContainer->begin();
    }

    const_iterator end() const {
        return setContainer->end();
    }

    bool add(const TYPE& type) {
        return setContainer->insert(type).second;
    }

    template <class ITER>
    void addAll(ITER first, ITER last) {
        setContainer->insert(first, last);
    }

    bool remove(const TYPE& type) {
        return (setContainer->erase(type) > 0);
    }

    iterator find(const TYPE& type) {
        return setContainer->find(type);
    }

    bool contains(const TYPE& type) const {
        return (setContainer->find(type) != setContainer->end());
    }

    bool equals(const this_type& other) const {
        return equals(other, std::equal_to<TYPE>());
    }

    template <class PRED>
    bool equals(const this_type& other, PRED comp) const {
        if (setContainer->size() != other.setContainer->size()) {
            return false;
        }
        return std::equal(setContainer->begin(), setContainer->end(), other.setContainer->begin(), comp);
    }

    void swap(this_type& other) {
        setContainer.swap(other->setContainer);
    }

    operator bool() const {
        return setContainer.get() != NULL;
    }

    bool operator! () const {
        return !setContainer;
    }

    bool operator== (const this_type& other) {
        return (setContainer == other.setContainer);
    }

    bool operator!= (const this_type& other) {
        return (setContainer != other.setContainer);
    }
};

}

#endif
