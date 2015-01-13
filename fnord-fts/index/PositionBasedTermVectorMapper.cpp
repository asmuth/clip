/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PositionBasedTermVectorMapper.h"

namespace Lucene {

PositionBasedTermVectorMapper::PositionBasedTermVectorMapper(bool ignoringOffsets) : TermVectorMapper(false, ignoringOffsets) {
    storeOffsets = false;
}

PositionBasedTermVectorMapper::~PositionBasedTermVectorMapper() {
}

bool PositionBasedTermVectorMapper::isIgnoringPositions() {
    return false;
}

void PositionBasedTermVectorMapper::map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) {
    for (int32_t i = 0; i < positions.size(); ++i) {
        TermVectorsPositionInfoPtr pos(currentPositions.get(positions[i]));
        if (!pos) {
            pos = newLucene<TermVectorsPositionInfo>(positions[i], storeOffsets);
            currentPositions.put(positions[i], pos);
        }
        pos->addTerm(term, offsets ? offsets[i] : TermVectorOffsetInfoPtr());
    }
}

void PositionBasedTermVectorMapper::setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions) {
    if (storePositions == false) {
        boost::throw_exception(RuntimeException(L"You must store positions in order to use this Mapper"));
    }
    if (storeOffsets == true) {
        // ignoring offsets
    }
    this->fieldToTerms = MapStringMapIntTermVectorsPositionInfo::newInstance();
    this->storeOffsets = storeOffsets;
    currentField = field;
    this->currentPositions = MapIntTermVectorsPositionInfo::newInstance();
    fieldToTerms.put(currentField, currentPositions);
}

MapStringMapIntTermVectorsPositionInfo PositionBasedTermVectorMapper::getFieldToTerms() {
    return fieldToTerms;
}

TermVectorsPositionInfo::TermVectorsPositionInfo(int32_t position, bool storeOffsets) {
    this->position = position;
    this->terms = Collection<String>::newInstance();
    if (storeOffsets) {
        offsets = Collection<TermVectorOffsetInfoPtr>::newInstance();
    }
}

TermVectorsPositionInfo::~TermVectorsPositionInfo() {
}

void TermVectorsPositionInfo::addTerm(const String& term, const TermVectorOffsetInfoPtr& info) {
    terms.add(term);
    if (offsets) {
        offsets.add(info);
    }
}

int32_t TermVectorsPositionInfo::getPosition() {
    return position;
}

Collection<String> TermVectorsPositionInfo::getTerms() {
    return terms;
}

Collection<TermVectorOffsetInfoPtr> TermVectorsPositionInfo::getOffsets() {
    return offsets;
}

}
