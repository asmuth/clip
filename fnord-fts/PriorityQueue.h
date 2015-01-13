/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "LuceneObject.h"
#include "MiscUtils.h"

namespace Lucene {

/// A PriorityQueue maintains a partial ordering of its elements such that the least element can always
/// be found in constant time.  Put()'s and pop()'s require log(size) time.
///
/// NOTE: This class pre-allocates a full array of length maxSize + 1.
template <typename TYPE>
class PriorityQueue : public LuceneObject {
public:
    typedef typename std::vector<TYPE> heap_type;

    PriorityQueue(int32_t maxSize) {
        this->_size = 0;
        this->_maxSize = maxSize;
    }

    virtual ~PriorityQueue() {
    }

protected:
    heap_type heap;
    int32_t _size;
    int32_t _maxSize;

public:
    virtual void initialize() {
        bool empty = heap.empty();

        if (empty) {
            int32_t heapSize = 0;
            if (_maxSize == 0) {
                // We allocate 1 extra to avoid if statement in top()
                heapSize = 2;
            } else if (_maxSize == INT_MAX) {
                // Don't wrap heapSize to -1, in this case, which causes a confusing NegativeArraySizeException.
                // Note that very likely this will simply then hit an OOME, but at least that's more indicative
                // to caller that this values is too big.  We don't +1 in this case, but it's very unlikely in
                // practice one will actually insert this many objects into the PQ
                heapSize = INT_MAX;
            } else {
                // NOTE: we add +1 because all access to heap is 1-based not 0-based.  heap[0] is unused.
                heapSize = _maxSize + 1;
            }
            this->heap.resize(heapSize);
        }

        // If sentinel objects are supported, populate the queue with them
        TYPE sentinel = getSentinelObject();
        if (empty && sentinel) {
            heap[1] = sentinel;
            for (int32_t i = 2; i < (int32_t)heap.size(); ++i) {
                heap[i] = getSentinelObject();
            }
            _size = _maxSize;
        }
    }

    /// Return maximum size of queue
    int32_t maxSize() {
        return _maxSize;
    }

    /// Adds an Object to a PriorityQueue in log(size) time.  If one tries to add more objects
    /// than maxSize from initialize an {@link IndexOutOfBoundsException} is thrown.
    TYPE add(const TYPE& type) {
        ++_size;
        if (_size < 0 || _size >= (int32_t)heap.size()) {
            boost::throw_exception(IndexOutOfBoundsException());
        }
        heap[_size] = type;
        upHeap();
        return heap[1];
    }

    /// Adds an Object to a PriorityQueue in log(size) time.  It returns the object (if any) that was
    /// dropped off the heap because it was full.  This can be the given parameter (in case it is
    /// smaller than the full heap's minimum, and couldn't be added), or another object that was
    /// previously the smallest value in the heap and now has been replaced by a larger one, or null
    /// if the queue wasn't yet full with maxSize elements.
    TYPE addOverflow(const TYPE& type) {
        if (_size < _maxSize) {
            add(type);
            return TYPE();
        } else if (_size > 0 && !lessThan(type, heap[1])) {
            TYPE result = heap[1];
            heap[1] = type;
            updateTop();
            return result;
        } else {
            return type;
        }
    }

    /// Returns the least element of the PriorityQueue.
    TYPE top() {
        // We don't need to check size here: if maxSize is 0, then heap is length 2 array with both
        // entries null.  If size is 0 then heap[1] is already null.
        return heap[1];
    }

    /// Removes and returns the least element of the PriorityQueue.
    TYPE pop() {
        if (_size > 0) {
            TYPE result = heap[1]; // save first value
            heap[1] = heap[_size]; // move last to first
            heap[_size--] = TYPE();
            downHeap(); // adjust heap
            return result;
        } else {
            return TYPE();
        }
    }

    /// Should be called when the Object at top changes values.
    TYPE updateTop() {
        downHeap();
        return heap[1];
    }

    /// Returns the number of elements currently stored in the PriorityQueue.
    int32_t size() const {
        return _size;
    }

    /// Returns whether PriorityQueue is currently empty.
    bool empty() const {
        return (_size == 0);
    }

    /// Removes all entries from the PriorityQueue.
    void clear() {
        for (int32_t i = 0; i <= _size; ++i) {
            heap[i] = TYPE();
        }
        _size = 0;
    }

protected:
    void upHeap() {
        int32_t i = _size;
        TYPE node = heap[i]; // save bottom node
        int32_t j = MiscUtils::unsignedShift(i, 1);
        while (j > 0 && lessThan(node, heap[j])) {
            heap[i] = heap[j]; // shift parents down
            i = j;
            j = MiscUtils::unsignedShift(j, 1);
        }
        heap[i] = node; // install saved node
    }

    void downHeap() {
        int32_t i = 1;
        TYPE node = heap[i]; // save top node
        int32_t j = i << 1; // find smaller child
        int32_t k = j + 1;
        if (k <= _size && lessThan(heap[k], heap[j])) {
            j = k;
        }
        while (j <= _size && lessThan(heap[j], node)) {
            heap[i] = heap[j]; // shift up child
            i = j;
            j = i << 1;
            k = j + 1;
            if (k <= _size && lessThan(heap[k], heap[j])) {
                j = k;
            }
        }
        heap[i] = node; // install saved node
    }

    /// Determines the ordering of objects in this priority queue.  Subclasses must define this one method.
    virtual bool lessThan(const TYPE& first, const TYPE& second) {
        return std::less<TYPE>()(first, second);
    }

    /// This method can be overridden by extending classes to return a sentinel object which will be used by
    /// {@link #initialize} to fill the queue, so that the code which uses that queue can always assume it's
    /// full and only change the top without attempting to insert any new object.
    ///
    /// Those sentinel values should always compare worse than any non-sentinel value (ie., {@link #lessThan}
    /// should always favour the non-sentinel values).
    virtual TYPE getSentinelObject() {
        return TYPE();
    }
};

}

#endif
