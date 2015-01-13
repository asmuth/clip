/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentTermPositionVector.h"
#include "TermVectorOffsetInfo.h"

namespace Lucene {

SegmentTermPositionVector::SegmentTermPositionVector(const String& field, Collection<String> terms,
        Collection<int32_t> termFreqs, Collection< Collection<int32_t> > positions,
        Collection< Collection<TermVectorOffsetInfoPtr> > offsets) :
    SegmentTermVector(field, terms, termFreqs) {
    this->offsets = offsets;
    this->positions = positions;
}

SegmentTermPositionVector::~SegmentTermPositionVector() {
}

const Collection<int32_t> SegmentTermPositionVector::EMPTY_TERM_POS() {
    static Collection<int32_t> _EMPTY_TERM_POS;
    if (!_EMPTY_TERM_POS) {
        _EMPTY_TERM_POS = Collection<int32_t>::newInstance();
    }
    return _EMPTY_TERM_POS;
}

Collection<TermVectorOffsetInfoPtr> SegmentTermPositionVector::getOffsets(int32_t index) {
    Collection<TermVectorOffsetInfoPtr> result(TermVectorOffsetInfo::EMPTY_OFFSET_INFO());
    if (!offsets) {
        return Collection<TermVectorOffsetInfoPtr>();
    }
    if (index >=0 && index < offsets.size()) {
        result = offsets[index];
    }
    return result;
}

Collection<int32_t> SegmentTermPositionVector::getTermPositions(int32_t index) {
    Collection<int32_t> result(EMPTY_TERM_POS());
    if (!positions) {
        return Collection<int32_t>();
    }
    if (index >= 0 && index < positions.size()) {
        result = positions[index];
    }
    return result;
}

}
