/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Fieldable.h"

namespace Lucene {

void Fieldable::setBoost(double boost) {
    BOOST_ASSERT(false);
    // override
}

double Fieldable::getBoost() {
    BOOST_ASSERT(false);
    return 0; // override
}

String Fieldable::name() {
    BOOST_ASSERT(false);
    return L""; // override
}

String Fieldable::stringValue() {
    BOOST_ASSERT(false);
    return L""; // override
}

ReaderPtr Fieldable::readerValue() {
    BOOST_ASSERT(false);
    return ReaderPtr(); // override
}

TokenStreamPtr Fieldable::tokenStreamValue() {
    BOOST_ASSERT(false);
    return TokenStreamPtr(); // override
}

bool Fieldable::isStored() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isIndexed() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isTokenized() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isTermVectorStored() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isStoreOffsetWithTermVector() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isStorePositionWithTermVector() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::isBinary() {
    BOOST_ASSERT(false);
    return false; // override
}

bool Fieldable::getOmitNorms() {
    BOOST_ASSERT(false);
    return false; // override
}

void Fieldable::setOmitNorms(bool omitNorms) {
    BOOST_ASSERT(false);
    // override
}

bool Fieldable::isLazy() {
    BOOST_ASSERT(false);
    return false; // override
}

int32_t Fieldable::getBinaryOffset() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t Fieldable::getBinaryLength() {
    BOOST_ASSERT(false);
    return 0; // override
}

ByteArray Fieldable::getBinaryValue() {
    BOOST_ASSERT(false);
    return ByteArray(); // override
}

ByteArray Fieldable::getBinaryValue(ByteArray result) {
    BOOST_ASSERT(false);
    return ByteArray(); // override
}

bool Fieldable::getOmitTermFreqAndPositions() {
    BOOST_ASSERT(false);
    return false; // override
}

void Fieldable::setOmitTermFreqAndPositions(bool omitTermFreqAndPositions) {
    BOOST_ASSERT(false);
    // override
}

}
