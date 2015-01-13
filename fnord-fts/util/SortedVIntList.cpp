/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SortedVIntList.h"
#include "_SortedVIntList.h"
#include "BitSet.h"
#include "OpenBitSet.h"
#include "DocIdSetIterator.h"
#include "MiscUtils.h"

namespace Lucene {

/// When a BitSet has fewer than 1 in BITS2VINTLIST_SIZE bits set, a SortedVIntList representing the
/// index numbers of the set bits will be smaller than that BitSet.
const int32_t SortedVIntList::BITS2VINTLIST_SIZE = 8;

const int32_t SortedVIntList::VB1 = 0x7f;
const int32_t SortedVIntList::BIT_SHIFT = 7;
const int32_t SortedVIntList::MAX_BYTES_PER_INT = (31 / SortedVIntList::BIT_SHIFT) + 1;

SortedVIntList::SortedVIntList(Collection<int32_t> sortedInts) {
    lastInt = 0;
    initBytes();
    for (int32_t i = 0; i < sortedInts.size(); ++i) {
        addInt(sortedInts[i]);
    }
    bytes.resize(lastBytePos);
}

SortedVIntList::SortedVIntList(Collection<int32_t> sortedInts, int32_t inputSize) {
    lastInt = 0;
    initBytes();
    for (int32_t i = 0; i < inputSize; ++i) {
        addInt(sortedInts[i]);
    }
    bytes.resize(lastBytePos);
}

SortedVIntList::SortedVIntList(const BitSetPtr& bits) {
    lastInt = 0;
    initBytes();
    int32_t nextInt = bits->nextSetBit(0);
    while (nextInt != -1) {
        addInt(nextInt);
        nextInt = bits->nextSetBit(nextInt + 1);
    }
    bytes.resize(lastBytePos);
}

SortedVIntList::SortedVIntList(const OpenBitSetPtr& bits) {
    lastInt = 0;
    initBytes();
    int32_t nextInt = bits->nextSetBit((int32_t)0);
    while (nextInt != -1) {
        addInt(nextInt);
        nextInt = bits->nextSetBit(nextInt + 1);
    }
    bytes.resize(lastBytePos);
}

SortedVIntList::SortedVIntList(const DocIdSetIteratorPtr& docIdSetIterator) {
    lastInt = 0;
    initBytes();
    int32_t doc;
    while ((doc = docIdSetIterator->nextDoc()) != DocIdSetIterator::NO_MORE_DOCS) {
        addInt(doc);
    }
    bytes.resize(lastBytePos);
}

SortedVIntList::~SortedVIntList() {
}

void SortedVIntList::initBytes() {
    _size = 0;
    bytes = ByteArray::newInstance(128); // initial byte size
    lastBytePos = 0;
}

void SortedVIntList::addInt(int32_t nextInt) {
    int32_t diff = nextInt - lastInt;
    if (diff < 0) {
        boost::throw_exception(IllegalArgumentException(L"Input not sorted or first element negative."));
    }

    if (!bytes || (lastBytePos + MAX_BYTES_PER_INT) > bytes.size()) {
        // biggest possible int does not fit
        bytes.resize((bytes.size() * 2) + MAX_BYTES_PER_INT);
    }

    // See IndexOutput.writeVInt()
    while ((diff & ~VB1) != 0) { // The high bit of the next byte needs to be set.
        bytes[lastBytePos++] = (uint8_t)((diff & VB1) | ~VB1);
        diff = MiscUtils::unsignedShift(diff, BIT_SHIFT);
    }
    bytes[lastBytePos++] = (uint8_t)diff; // Last byte, high bit not set.
    ++_size;
    lastInt = nextInt;
}

int32_t SortedVIntList::size() {
    return _size;
}

int32_t SortedVIntList::getByteSize() {
    return bytes ? bytes.size() : 0;
}

bool SortedVIntList::isCacheable() {
    return true;
}

DocIdSetIteratorPtr SortedVIntList::iterator() {
    return newLucene<SortedDocIdSetIterator>(shared_from_this());
}

SortedDocIdSetIterator::SortedDocIdSetIterator(const SortedVIntListPtr& list) {
    _list = list;
    bytePos = 0;
    lastInt = 0;
    doc = -1;
}

SortedDocIdSetIterator::~SortedDocIdSetIterator() {
}

void SortedDocIdSetIterator::advance() {
    SortedVIntListPtr list(_list);

    // See IndexInput.readVInt()
    uint8_t b = list->bytes[bytePos++];
    lastInt += b & list->VB1;
    for (int32_t s = list->BIT_SHIFT; (b & ~list->VB1) != 0; s += list->BIT_SHIFT) {
        b = list->bytes[bytePos++];
        lastInt += (b & list->VB1) << s;
    }
}

int32_t SortedDocIdSetIterator::docID() {
    return doc;
}

int32_t SortedDocIdSetIterator::nextDoc() {
    SortedVIntListPtr list(_list);
    if (bytePos >= list->lastBytePos) {
        doc = NO_MORE_DOCS;
    } else {
        advance();
        doc = lastInt;
    }
    return doc;
}

int32_t SortedDocIdSetIterator::advance(int32_t target) {
    SortedVIntListPtr list(_list);
    while (bytePos < list->lastBytePos) {
        advance();
        if (lastInt >= target) {
            doc = lastInt;
            return doc;
        }
    }
    doc = NO_MORE_DOCS;
    return doc;
}

}
