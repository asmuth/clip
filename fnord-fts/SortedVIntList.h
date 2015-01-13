/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SORTEDVINTLIST_H
#define SORTEDVINTLIST_H

#include "DocIdSet.h"

namespace Lucene {

/// Stores and iterate on sorted integers in compressed form in RAM.
///
/// The code for compressing the differences between ascending integers was borrowed from {@link IndexInput}
/// and {@link IndexOutput}.
///
/// NOTE: this class assumes the stored integers are doc Ids (hence why it extends {@link DocIdSet}). Therefore
/// its {@link #iterator()} assumes {@link DocIdSetIterator#NO_MORE_DOCS} can be used as sentinel.  If you
/// intend to use this value, then make sure it's not used during search flow.
class SortedVIntList : public DocIdSet {
public:
    /// Create a SortedVIntList from all elements of an array of integers.
    /// @param sortedInts A sorted array of non negative integers.
    SortedVIntList(Collection<int32_t> sortedInts);

    /// Create a SortedVIntList from an array of integers.
    /// @param sortedInts A sorted array of non negative integers.
    /// @param inputSize The number of integers to be used from the array.
    SortedVIntList(Collection<int32_t> sortedInts, int32_t inputSize);

    /// Create a SortedVIntList from a BitSet.
    /// @param bits A bit set representing a set of integers.
    SortedVIntList(const BitSetPtr& bits);

    /// Create a SortedVIntList from an OpenBitSet.
    /// @param bits A bit set representing a set of integers.
    SortedVIntList(const OpenBitSetPtr& bits);

    /// Create a SortedVIntList.
    /// @param docIdSetIterator An iterator providing document numbers as a set of integers.
    /// This DocIdSetIterator is iterated completely when this constructor is called and it must provide the
    /// integers in non decreasing order.
    SortedVIntList(const DocIdSetIteratorPtr& docIdSetIterator);

    virtual ~SortedVIntList();

    LUCENE_CLASS(SortedVIntList);

public:
    /// When a BitSet has fewer than 1 in BITS2VINTLIST_SIZE bits set, a SortedVIntList representing the
    /// index numbers of the set bits will be smaller than that BitSet.
    static const int32_t BITS2VINTLIST_SIZE;

protected:
    static const int32_t VB1;
    static const int32_t BIT_SHIFT;
    static const int32_t MAX_BYTES_PER_INT;

    int32_t _size;
    ByteArray bytes;
    int32_t lastBytePos;
    int32_t lastInt;

public:
    /// @return The total number of sorted integers.
    int32_t size();

    /// @return The size of the byte array storing the compressed sorted integers.
    int32_t getByteSize();

    /// This DocIdSet implementation is cacheable.
    virtual bool isCacheable();

    /// @return An iterator over the sorted integers.
    virtual DocIdSetIteratorPtr iterator();

protected:
    void initBytes();
    void addInt(int32_t nextInt);

    friend class SortedDocIdSetIterator;
};

}

#endif
