/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/analysis/tokenattributes/PositionIncrementAttribute.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

PositionIncrementAttribute::PositionIncrementAttribute() {
    positionIncrement = 1;
}

PositionIncrementAttribute::~PositionIncrementAttribute() {
}

String PositionIncrementAttribute::toString() {
    return L"positionIncrement=" + StringUtils::toString(positionIncrement);
}

void PositionIncrementAttribute::setPositionIncrement(int32_t positionIncrement) {
    if (positionIncrement < 0) {
        boost::throw_exception(IllegalArgumentException(L"Increment must be zero or greater: " + StringUtils::toString(positionIncrement)));
    }
    this->positionIncrement = positionIncrement;
}

int32_t PositionIncrementAttribute::getPositionIncrement() {
    return positionIncrement;
}

void PositionIncrementAttribute::clear() {
    this->positionIncrement = 1;
}

bool PositionIncrementAttribute::equals(const LuceneObjectPtr& other) {
    if (Attribute::equals(other)) {
        return true;
    }

    PositionIncrementAttributePtr otherPositionIncrementAttribute(std::dynamic_pointer_cast<PositionIncrementAttribute>(other));
    if (otherPositionIncrementAttribute) {
        return positionIncrement == otherPositionIncrementAttribute->positionIncrement;
    }

    return false;
}

int32_t PositionIncrementAttribute::hashCode() {
    return positionIncrement;
}

void PositionIncrementAttribute::copyTo(const AttributePtr& target) {
    PositionIncrementAttributePtr targetPositionIncrementAttribute(std::dynamic_pointer_cast<PositionIncrementAttribute>(target));
    targetPositionIncrementAttribute->setPositionIncrement(positionIncrement);
}

LuceneObjectPtr PositionIncrementAttribute::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<PositionIncrementAttribute>();
    PositionIncrementAttributePtr cloneAttribute(std::dynamic_pointer_cast<PositionIncrementAttribute>(Attribute::clone(clone)));
    cloneAttribute->positionIncrement = positionIncrement;
    return cloneAttribute;
}

}
