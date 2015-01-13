/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "OpenBitSetDISI.h"

namespace Lucene {

OpenBitSetDISI::OpenBitSetDISI(const DocIdSetIteratorPtr& disi, int32_t maxSize) : OpenBitSet(maxSize) {
    inPlaceOr(disi);
}

OpenBitSetDISI::OpenBitSetDISI(int32_t maxSize) : OpenBitSet(maxSize) {
}

OpenBitSetDISI::~OpenBitSetDISI() {
}

void OpenBitSetDISI::inPlaceOr(const DocIdSetIteratorPtr& disi) {
    int32_t doc;
    int32_t _size = size();
    while ((doc = disi->nextDoc()) < _size) {
        set(doc);
    }
}

void OpenBitSetDISI::inPlaceAnd(const DocIdSetIteratorPtr& disi) {
    int32_t bitSetDoc = nextSetBit((int32_t)0);
    int32_t disiDoc;
    while (bitSetDoc != -1 && (disiDoc = disi->advance(bitSetDoc)) != DocIdSetIterator::NO_MORE_DOCS) {
        clear(bitSetDoc, disiDoc);
        bitSetDoc = nextSetBit(disiDoc + 1);
    }
    if (bitSetDoc != -1) {
        clear((int64_t)bitSetDoc, size());
    }
}

void OpenBitSetDISI::inPlaceNot(const DocIdSetIteratorPtr& disi) {
    int32_t doc;
    int32_t _size = size();
    while ((doc = disi->nextDoc()) < _size) {
        clear(doc);
    }
}

void OpenBitSetDISI::inPlaceXor(const DocIdSetIteratorPtr& disi) {
    int32_t doc;
    int32_t _size = size();
    while ((doc = disi->nextDoc()) < _size) {
        flip(doc);
    }
}

}
