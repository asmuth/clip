/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Payload.h"
#include "MiscUtils.h"

namespace Lucene {

Payload::Payload() {
    this->offset = 0;
    this->_length = 0;
}

Payload::Payload(ByteArray data) {
    this->data = data;
    this->offset = 0;
    this->_length = data.size();
}

Payload::Payload(ByteArray data, int32_t offset, int32_t length) {
    if (offset < 0 || offset + length > data.size()) {
        boost::throw_exception(IllegalArgumentException());
    }
    this->data = data;
    this->offset = offset;
    this->_length = length;
}

Payload::~Payload() {
}

void Payload::setData(ByteArray data) {
    setData(data, 0, data.size());
}

void Payload::setData(ByteArray data, int32_t offset, int32_t length) {
    this->data = data;
    this->offset = offset;
    this->_length = length;
}

ByteArray Payload::getData() {
    return this->data;
}

int32_t Payload::getOffset() {
    return this->offset;
}

int32_t Payload::length() {
    return this->_length;
}

uint8_t Payload::byteAt(int32_t index) {
    if (0 <= index && index < this->_length) {
        return this->data[this->offset + index];
    }
    boost::throw_exception(IndexOutOfBoundsException());
    return 0;
}

ByteArray Payload::toByteArray() {
    ByteArray retArray(ByteArray::newInstance(this->_length));
    MiscUtils::arrayCopy(this->data.get(), this->offset, retArray.get(), 0, this->_length);
    return retArray;
}

void Payload::copyTo(ByteArray target, int32_t targetOffset) {
    if (this->_length > target.size() + targetOffset) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    MiscUtils::arrayCopy(this->data.get(), this->offset, target.get(), targetOffset, this->_length);
}

LuceneObjectPtr Payload::clone(const LuceneObjectPtr& other) {
    // Start with a shallow copy of data
    LuceneObjectPtr clone = LuceneObject::clone(other ? other : newLucene<Payload>());
    PayloadPtr clonePayload(boost::dynamic_pointer_cast<Payload>(clone));
    clonePayload->offset = offset;
    clonePayload->_length = _length;

    // Only copy the part of data that belongs to this Payload
    if (offset == 0 && _length == data.size()) {
        // It is the whole thing, so just clone it.
        clonePayload->data = ByteArray::newInstance(data.size());
        MiscUtils::arrayCopy(data.get(), 0, clonePayload->data.get(), 0, data.size());
    } else {
        // Just get the part
        clonePayload->data = toByteArray();
        clonePayload->offset = 0;
    }
    return clonePayload;
}

bool Payload::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    PayloadPtr otherPayload(boost::dynamic_pointer_cast<Payload>(other));
    if (otherPayload) {
        if (_length == otherPayload->_length) {
            return (std::memcmp(data.get(), otherPayload->data.get(), _length) == 0);
        } else {
            return false;
        }
    }

    return false;
}

int32_t Payload::hashCode() {
    return MiscUtils::hashCode(data.get(), offset, offset + _length);
}

}
