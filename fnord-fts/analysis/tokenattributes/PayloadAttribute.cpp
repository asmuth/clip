/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PayloadAttribute.h"
#include "Payload.h"
#include "StringUtils.h"

namespace Lucene {

PayloadAttribute::PayloadAttribute() {
}

PayloadAttribute::PayloadAttribute(const PayloadPtr& payload) {
    this->payload = payload;
}

PayloadAttribute::~PayloadAttribute() {
}

String PayloadAttribute::toString() {
    return L"payload(length)=" + StringUtils::toString(payload->length());
}

PayloadPtr PayloadAttribute::getPayload() {
    return this->payload;
}

void PayloadAttribute::setPayload(const PayloadPtr& payload) {
    this->payload = payload;
}

void PayloadAttribute::clear() {
    payload.reset();
}

LuceneObjectPtr PayloadAttribute::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Attribute::clone(other ? other : newLucene<PayloadAttribute>());
    PayloadAttributePtr cloneAttribute(boost::dynamic_pointer_cast<PayloadAttribute>(clone));
    if (payload) {
        cloneAttribute->payload = boost::dynamic_pointer_cast<Payload>(payload->clone());
    }
    return cloneAttribute;
}

bool PayloadAttribute::equals(const LuceneObjectPtr& other) {
    if (Attribute::equals(other)) {
        return true;
    }

    PayloadAttributePtr otherAttribute(boost::dynamic_pointer_cast<PayloadAttribute>(other));
    if (otherAttribute) {
        if (!otherAttribute->payload && !payload) {
            return true;
        }
        return otherAttribute->payload->equals(payload);
    }

    return false;
}

int32_t PayloadAttribute::hashCode() {
    return payload ? payload->hashCode() : 0;
}

void PayloadAttribute::copyTo(const AttributePtr& target) {
    PayloadAttributePtr targetPayloadAttribute(boost::dynamic_pointer_cast<PayloadAttribute>(target));
    targetPayloadAttribute->setPayload(payload ? boost::dynamic_pointer_cast<Payload>(payload->clone()) : PayloadPtr());
}

}
