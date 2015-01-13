/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldInvertState.h"

namespace Lucene {

FieldInvertState::FieldInvertState(int32_t position, int32_t length, int32_t numOverlap, int32_t offset, double boost) {
    this->position = position;
    this->length = length;
    this->numOverlap = numOverlap;
    this->offset = offset;
    this->boost = boost;
}

FieldInvertState::~FieldInvertState() {
}

void FieldInvertState::reset(double docBoost) {
    position = 0;
    length = 0;
    numOverlap = 0;
    offset = 0;
    boost = docBoost;
    attributeSource.reset();
}

int32_t FieldInvertState::getPosition() {
    return position;
}

int32_t FieldInvertState::getLength() {
    return length;
}

int32_t FieldInvertState::getNumOverlap() {
    return numOverlap;
}

int32_t FieldInvertState::getOffset() {
    return offset;
}

double FieldInvertState::getBoost() {
    return boost;
}

AttributeSourcePtr FieldInvertState::getAttributeSource() {
    return attributeSource;
}

}
