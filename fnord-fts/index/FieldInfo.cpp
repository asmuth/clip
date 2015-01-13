/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldInfo.h"

namespace Lucene {

FieldInfo::FieldInfo(const String& name, bool isIndexed, int32_t number, bool storeTermVector, bool storePositionWithTermVector,
                     bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions) {
    this->name = name;
    this->isIndexed = isIndexed;
    this->number = number;

    // for non-indexed fields, leave defaults
    this->storeTermVector = isIndexed ? storeTermVector : false;
    this->storeOffsetWithTermVector = isIndexed ? storeOffsetWithTermVector : false;
    this->storePositionWithTermVector = isIndexed ? storePositionWithTermVector : false;
    this->storePayloads = isIndexed ? storePayloads : false;
    this->omitNorms = isIndexed ? omitNorms : true;
    this->omitTermFreqAndPositions = isIndexed ? omitTermFreqAndPositions : false;
}

FieldInfo::~FieldInfo() {
}

LuceneObjectPtr FieldInfo::clone(const LuceneObjectPtr& other) {
    return newLucene<FieldInfo>(name, isIndexed, number, storeTermVector, storePositionWithTermVector,
                                storeOffsetWithTermVector, omitNorms, storePayloads, omitTermFreqAndPositions);
}

void FieldInfo::update(bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                       bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads,
                       bool omitTermFreqAndPositions) {
    if (this->isIndexed != isIndexed) {
        this->isIndexed = true;    // once indexed, always index
    }
    if (isIndexed) { // if updated field data is not for indexing, leave the updates out
        if (this->storeTermVector != storeTermVector) {
            this->storeTermVector = true;    // once vector, always vector
        }
        if (this->storePositionWithTermVector != storePositionWithTermVector) {
            this->storePositionWithTermVector = true;    // once vector, always vector
        }
        if (this->storeOffsetWithTermVector != storeOffsetWithTermVector) {
            this->storeOffsetWithTermVector = true;    // once vector, always vector
        }
        if (this->storePayloads != storePayloads) {
            this->storePayloads = true;
        }
        if (this->omitNorms != omitNorms) {
            this->omitNorms = false;    // once norms are stored, always store
        }
        if (this->omitTermFreqAndPositions != omitTermFreqAndPositions) {
            this->omitTermFreqAndPositions = true;    // if one require omitTermFreqAndPositions at least once, it remains off for life
        }
    }
}

}
