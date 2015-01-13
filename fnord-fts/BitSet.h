/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BITSET_H
#define BITSET_H

#include <boost/dynamic_bitset.hpp>
#include "LuceneObject.h"

namespace Lucene {

class BitSet : public LuceneObject {
public:
    BitSet(uint32_t size = 0);
    virtual ~BitSet();

    LUCENE_CLASS(BitSet);

protected:
    typedef boost::dynamic_bitset<uint64_t> bitset_type;
    bitset_type bitSet;

public:
    const uint64_t* getBits();
    void clear();
    void clear(uint32_t bitIndex);
    void fastClear(uint32_t bitIndex);
    void clear(uint32_t fromIndex, uint32_t toIndex);
    void fastClear(uint32_t fromIndex, uint32_t toIndex);
    void set(uint32_t bitIndex);
    void fastSet(uint32_t bitIndex);
    void set(uint32_t bitIndex, bool value);
    void fastSet(uint32_t bitIndex, bool value);
    void set(uint32_t fromIndex, uint32_t toIndex);
    void fastSet(uint32_t fromIndex, uint32_t toIndex);
    void set(uint32_t fromIndex, uint32_t toIndex, bool value);
    void fastSet(uint32_t fromIndex, uint32_t toIndex, bool value);
    void flip(uint32_t bitIndex);
    void fastFlip(uint32_t bitIndex);
    void flip(uint32_t fromIndex, uint32_t toIndex);
    void fastFlip(uint32_t fromIndex, uint32_t toIndex);
    uint32_t size() const;
    uint32_t numBlocks() const;
    bool isEmpty() const;
    bool get(uint32_t bitIndex) const;
    bool fastGet(uint32_t bitIndex) const;
    int32_t nextSetBit(uint32_t fromIndex) const;
    void _and(const BitSetPtr& set);
    void _or(const BitSetPtr& set);
    void _xor(const BitSetPtr& set);
    void andNot(const BitSetPtr& set);
    bool intersectsBitSet(const BitSetPtr& set) const;
    uint32_t cardinality();
    void resize(uint32_t size);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
