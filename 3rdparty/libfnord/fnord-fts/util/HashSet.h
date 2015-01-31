/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef HASHSET_H
#define HASHSET_H

#include <boost/unordered_set.hpp>
#include "fnord-fts/util/LuceneSync.h"

namespace fnord {
namespace fts {

/// Utility template class to handle hash set collections that can be safely copied and shared
template < class TYPE, class HASH = boost::hash<TYPE>, class EQUAL = std::equal_to<TYPE> >
class HashSet : public LuceneSync {
public:
    typedef HashSet<TYPE, HASH, EQUAL> this_type;
    typedef boost::unordered_set<TYPE, HASH, EQUAL> set_type;
    typedef typename set_type::iterator iterator;
    typedef typename set_type::const_iterator const_iterator;
    typedef TYPE value_type;

    virtual ~HashSet() {
    }

protected:
    std::shared_ptr<set_type> setContainer;

public:
    static this_type newInstance() {
        this_type instance;
        instance.setContainer = fnord::fts::newInstance<set_type>();
        return instance;
    }

    template <class ITER>
    static this_type newInstance(ITER first, ITER last) {
        this_type instance;
        instance.setContainer = fnord::fts::newInstance<set_type>(first, last);
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

    operator bool() const {
        return setContainer.get() != NULL;
    }

    bool operator! () const {
        return !setContainer;
    }

    set_type& operator= (const set_type& other) {
        setContainer = other.setContainer;
        return *this;
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
};

}

}
#endif
