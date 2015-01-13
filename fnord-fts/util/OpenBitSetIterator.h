/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef OPENBITSETITERATOR_H
#define OPENBITSETITERATOR_H

#include "fnord-fts/search/DocIdSetIterator.h"

namespace Lucene {

/// An iterator to iterate over set bits in an OpenBitSet.
/// This is faster than nextSetBit() for iterating over the complete set of bits,
/// especially when the density of the bits set is high.
class OpenBitSetIterator : public DocIdSetIterator {
public:
    OpenBitSetIterator(const OpenBitSetPtr& bitSet);
    OpenBitSetIterator(LongArray bits, int32_t numWords);
    virtual ~OpenBitSetIterator();

    LUCENE_CLASS(OpenBitSetIterator);

protected:
    LongArray arr;
    int32_t words;
    int32_t i;
    int64_t word;
    int32_t wordShift;
    int32_t indexArray;
    int32_t curDocId;

    /// The General Idea: instead of having an array per byte that has the offsets of the
    /// next set bit, that array could be packed inside a 32 bit integer (8 4 bit numbers).
    /// That should be faster than accessing an array for each index, and the total array
    /// size is kept smaller (256*sizeof(int32_t))=1K
    static const int32_t bitlist[];

public:
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();

protected:
    /// 64 bit shifts
    void shift();
};

}

#endif
