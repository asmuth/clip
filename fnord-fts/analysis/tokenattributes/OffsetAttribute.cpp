/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "OffsetAttribute.h"
#include "StringUtils.h"

namespace Lucene {

OffsetAttribute::OffsetAttribute() {
    _startOffset = 0;
    _endOffset = 0;
}

OffsetAttribute::~OffsetAttribute() {
}

String OffsetAttribute::toString() {
    return L"startOffset=" + StringUtils::toString(_startOffset) + L";endOffset=" + StringUtils::toString(_endOffset);
}

int32_t OffsetAttribute::startOffset() {
    return _startOffset;
}

void OffsetAttribute::setOffset(int32_t startOffset, int32_t endOffset) {
    this->_startOffset = startOffset;
    this->_endOffset = endOffset;
}

int32_t OffsetAttribute::endOffset() {
    return _endOffset;
}

void OffsetAttribute::clear() {
    _startOffset = 0;
    _endOffset = 0;
}

bool OffsetAttribute::equals(const LuceneObjectPtr& other) {
    if (Attribute::equals(other)) {
        return true;
    }

    OffsetAttributePtr otherOffsetAttribute(boost::dynamic_pointer_cast<OffsetAttribute>(other));
    if (otherOffsetAttribute) {
        return (otherOffsetAttribute->_startOffset == _startOffset && otherOffsetAttribute->_endOffset == _endOffset);
    }

    return false;
}

int32_t OffsetAttribute::hashCode() {
    int32_t code = _startOffset;
    code = code * 31 + _endOffset;
    return code;
}

void OffsetAttribute::copyTo(const AttributePtr& target) {
    OffsetAttributePtr targetOffsetAttribute(boost::dynamic_pointer_cast<OffsetAttribute>(target));
    targetOffsetAttribute->setOffset(_startOffset, _endOffset);
}

LuceneObjectPtr OffsetAttribute::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<OffsetAttribute>();
    OffsetAttributePtr cloneAttribute(boost::dynamic_pointer_cast<OffsetAttribute>(Attribute::clone(clone)));
    cloneAttribute->_startOffset = _startOffset;
    cloneAttribute->_endOffset = _endOffset;
    return cloneAttribute;
}

}
