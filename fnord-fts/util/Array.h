/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ARRAY_H
#define ARRAY_H

#include <cstring>
#include "fnord-fts/fts.h"

namespace fnord {
namespace fts {

template <typename TYPE>
class ArrayData {
public:
    ArrayData(int32_t size_) {
        data = NULL;
        resize(size_);
    }

    ~ArrayData() {
        resize(0);
    }

public:
    TYPE* data;
    int32_t size;

public:
    void resize(int32_t size_) {
        if (size_ == 0) {
            FreeMemory(data);
            data = NULL;
        } else if (data == NULL) {
            data = (TYPE*)AllocMemory(size_ * sizeof(TYPE));
        } else {
            data = (TYPE*)ReallocMemory(data, size_ * sizeof(TYPE));
        }
        this->size = size_;
    }
};

/// Utility template class to handle sharable arrays of simple data types
template <typename TYPE>
class Array {
public:
    typedef Array<TYPE> this_type;
    typedef ArrayData<TYPE> array_type;

    Array() {
        array = NULL;
    }

protected:
    std::shared_ptr<array_type> container;
    array_type* array;

public:
    static this_type newInstance(int32_t size) {
        this_type instance;
        instance.container = fnord::fts::newInstance<array_type>(size);
        instance.array = instance.container.get();
        return instance;
    }

    void reset() {
        resize(0);
    }

    void resize(int32_t size) {
        if (size == 0) {
            container.reset();
        } else if (!container) {
            container = fnord::fts::newInstance<array_type>(size);
        } else {
            container->resize(size);
        }
        array = container.get();
    }

    TYPE* get() const {
        return array->data;
    }

    int32_t size() const {
        return array->size;
    }

    bool equals(const this_type& other) const {
        if (array->size != other.array->size) {
            return false;
        }
        return (std::memcmp(array->data, other.array->data, array->size) == 0);
    }

    int32_t hashCode() const {
        return (int32_t)(int64_t)array;
    }

    TYPE& operator[] (int32_t i) const {
        BOOST_ASSERT(i >= 0 && i < array->size);
        return array->data[i];
    }

    operator bool () const {
        return container.get() != NULL;
    }

    bool operator! () const {
        return !container;
    }

    bool operator== (const Array<TYPE>& other) {
        return (container == other.container);
    }

    bool operator!= (const Array<TYPE>& other) {
        return (container != other.container);
    }
};

template <class TYPE>
inline std::size_t hash_value(const Array<TYPE>& value) {
    return (std::size_t)value.hashCode();
}

template <class TYPE>
inline bool operator== (const Array<TYPE>& value1, const Array<TYPE>& value2) {
    return (value1.hashCode() == value2.hashCode());
}

}

}
#endif
