/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorOffsetInfo.h"

namespace Lucene {

TermVectorOffsetInfo::TermVectorOffsetInfo(int32_t startOffset, int32_t endOffset) {
    this->endOffset = endOffset;
    this->startOffset = startOffset;
}

TermVectorOffsetInfo::~TermVectorOffsetInfo() {
}

const Collection<TermVectorOffsetInfoPtr> TermVectorOffsetInfo::EMPTY_OFFSET_INFO() {
    static Collection<TermVectorOffsetInfoPtr> _EMPTY_OFFSET_INFO;
    if (!_EMPTY_OFFSET_INFO) {
        _EMPTY_OFFSET_INFO = Collection<TermVectorOffsetInfoPtr>::newInstance();
    }
    return _EMPTY_OFFSET_INFO;
}

int32_t TermVectorOffsetInfo::getEndOffset() {
    return endOffset;
}

void TermVectorOffsetInfo::setEndOffset(int32_t endOffset) {
    this->endOffset = endOffset;
}

int32_t TermVectorOffsetInfo::getStartOffset() {
    return startOffset;
}

void TermVectorOffsetInfo::setStartOffset(int32_t startOffset) {
    this->startOffset = startOffset;
}

bool TermVectorOffsetInfo::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    TermVectorOffsetInfoPtr otherTermVector(boost::dynamic_pointer_cast<TermVectorOffsetInfo>(other));
    if (!otherTermVector) {
        return false;
    }
    return (endOffset == otherTermVector->endOffset && startOffset == otherTermVector->startOffset);
}

int32_t TermVectorOffsetInfo::hashCode() {
    int32_t result = startOffset;
    return (29 * result + endOffset);
}

}
