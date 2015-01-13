/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocIdBitSet.h"
#include "_DocIdBitSet.h"
#include "BitSet.h"

namespace Lucene {

DocIdBitSet::DocIdBitSet() {
}

DocIdBitSet::DocIdBitSet(const BitSetPtr& bitSet) {
    this->bitSet = bitSet;
}

DocIdBitSet::~DocIdBitSet() {
}

DocIdSetIteratorPtr DocIdBitSet::iterator() {
    return newLucene<DocIdBitSetIterator>(bitSet);
}

bool DocIdBitSet::isCacheable() {
    return true;
}

BitSetPtr DocIdBitSet::getBitSet() {
    return bitSet;
}

bool DocIdBitSet::equals(const LuceneObjectPtr& other) {
    if (DocIdSet::equals(other)) {
        return true;
    }
    DocIdBitSetPtr otherBitSet(boost::dynamic_pointer_cast<DocIdBitSet>(other));
    return bitSet->equals(otherBitSet->bitSet);
}

int32_t DocIdBitSet::hashCode() {
    return bitSet->hashCode();
}

LuceneObjectPtr DocIdBitSet::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<DocIdBitSet>();
    DocIdBitSetPtr cloneBitSet(boost::dynamic_pointer_cast<DocIdBitSet>(LuceneObject::clone(clone)));
    cloneBitSet->bitSet = boost::dynamic_pointer_cast<BitSet>(bitSet->clone());
    return cloneBitSet;
}

DocIdBitSetIterator::DocIdBitSetIterator(const BitSetPtr& bitSet) {
    this->bitSet = bitSet;
    this->docId = -1;
}

DocIdBitSetIterator::~DocIdBitSetIterator() {
}

int32_t DocIdBitSetIterator::docID() {
    return docId;
}

int32_t DocIdBitSetIterator::nextDoc() {
    int32_t doc = bitSet->nextSetBit(docId + 1);
    docId = doc == -1 ? NO_MORE_DOCS : doc;
    return docId;
}

int32_t DocIdBitSetIterator::advance(int32_t target) {
    int32_t doc = bitSet->nextSetBit(target);
    docId = doc == -1 ? NO_MORE_DOCS : doc;
    return docId;
}

}
