/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/SpanFilterResult.h"

namespace fnord {
namespace fts {

SpanFilterResult::SpanFilterResult(const DocIdSetPtr& docIdSet, Collection<PositionInfoPtr> positions) {
    this->docIdSet = docIdSet;
    this->positions = positions;
}

SpanFilterResult::~SpanFilterResult() {
}

Collection<PositionInfoPtr> SpanFilterResult::getPositions() {
    return positions;
}

DocIdSetPtr SpanFilterResult::getDocIdSet() {
    return docIdSet;
}

PositionInfo::PositionInfo(int32_t doc) {
    this->doc = doc;
    this->positions = Collection<StartEndPtr>::newInstance();
}

PositionInfo::~PositionInfo() {
}

void PositionInfo::addPosition(int32_t start, int32_t end) {
    positions.add(newLucene<StartEnd>(start, end));
}

int32_t PositionInfo::getDoc() {
    return doc;
}

Collection<StartEndPtr> PositionInfo::getPositions() {
    return positions;
}

StartEnd::StartEnd(int32_t start, int32_t end) {
    this->start = start;
    this->end = end;
}

StartEnd::~StartEnd() {
}

int32_t StartEnd::getEnd() {
    return end;
}

int32_t StartEnd::getStart() {
    return start;
}

}

}
