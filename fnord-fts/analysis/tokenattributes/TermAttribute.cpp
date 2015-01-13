/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermAttribute.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t TermAttribute::MIN_BUFFER_SIZE = 10;

TermAttribute::TermAttribute() {
    _termLength = 0;
}

TermAttribute::~TermAttribute() {
}

String TermAttribute::toString() {
    return L"term=" + term();
}

String TermAttribute::term() {
    initTermBuffer();
    return String(_termBuffer.get(), _termLength);
}

void TermAttribute::setTermBuffer(const wchar_t* buffer, int32_t offset, int32_t length) {
    growTermBuffer(length);
    MiscUtils::arrayCopy(buffer, offset, _termBuffer.get(), 0, length);
    _termLength = length;
}

void TermAttribute::setTermBuffer(const String& buffer) {
    int32_t length = (int32_t)buffer.size();
    growTermBuffer(length);
    MiscUtils::arrayCopy(buffer.begin(), 0, _termBuffer.get(), 0, length);
    _termLength = length;
}

CharArray TermAttribute::termBuffer() {
    if (!_termBuffer) {
        initTermBuffer();
    }
    return _termBuffer;
}

wchar_t* TermAttribute::termBufferArray() {
    if (!_termBuffer) {
        initTermBuffer();
    }
    return _termBuffer.get();
}

CharArray TermAttribute::resizeTermBuffer(int32_t newSize) {
    if (!_termBuffer) {
        // The buffer is always at least MIN_BUFFER_SIZE
        _termBuffer = CharArray::newInstance(MiscUtils::getNextSize(std::max(newSize, MIN_BUFFER_SIZE)));
    } else if (_termBuffer.size() < newSize) {
        _termBuffer.resize(MiscUtils::getNextSize(newSize));
    }
    return _termBuffer;
}

void TermAttribute::growTermBuffer(int32_t newSize) {
    if (!_termBuffer) {
        // The buffer is always at least MIN_BUFFER_SIZE
        _termBuffer = CharArray::newInstance(MiscUtils::getNextSize(std::max(newSize, MIN_BUFFER_SIZE)));
    } else if (_termBuffer.size() < newSize) {
        _termBuffer.resize(MiscUtils::getNextSize(newSize));
    }
}

void TermAttribute::initTermBuffer() {
    if (!_termBuffer) {
        _termBuffer = CharArray::newInstance(MiscUtils::getNextSize(MIN_BUFFER_SIZE));
        _termLength = 0;
    }
}

int32_t TermAttribute::termLength() {
    return _termLength;
}

void TermAttribute::setTermLength(int32_t length) {
    if (!_termBuffer) {
        initTermBuffer();
    }
    if (length > _termBuffer.size()) {
        boost::throw_exception(IllegalArgumentException(L"length " + StringUtils::toString(length) +
                               L" exceeds the size of the termBuffer (" +
                               StringUtils::toString(_termBuffer.size()) + L")"));
    }
    _termLength = length;
}

int32_t TermAttribute::hashCode() {
    initTermBuffer();
    int32_t code = _termLength;
    code = code * 31 + MiscUtils::hashCode(_termBuffer.get(), 0, _termLength);
    return code;
}

void TermAttribute::clear() {
    _termLength = 0;
}

LuceneObjectPtr TermAttribute::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Attribute::clone(other ? other : newLucene<TermAttribute>());
    TermAttributePtr cloneAttribute(boost::dynamic_pointer_cast<TermAttribute>(clone));
    cloneAttribute->_termLength = _termLength;
    if (_termBuffer) {
        cloneAttribute->_termBuffer = CharArray::newInstance(_termBuffer.size());
        MiscUtils::arrayCopy(_termBuffer.get(), 0, cloneAttribute->_termBuffer.get(), 0, _termBuffer.size());
    }
    return cloneAttribute;
}

bool TermAttribute::equals(const LuceneObjectPtr& other) {
    if (Attribute::equals(other)) {
        return true;
    }

    TermAttributePtr otherTermAttribute(boost::dynamic_pointer_cast<TermAttribute>(other));
    if (otherTermAttribute) {
        initTermBuffer();
        otherTermAttribute->initTermBuffer();

        if (_termLength != otherTermAttribute->_termLength) {
            return false;
        }

        return (std::memcmp(_termBuffer.get(), otherTermAttribute->_termBuffer.get(), _termLength) == 0);
    }

    return false;
}

void TermAttribute::copyTo(const AttributePtr& target) {
    initTermBuffer();
    TermAttributePtr targetTermAttribute(boost::dynamic_pointer_cast<TermAttribute>(target));
    targetTermAttribute->setTermBuffer(_termBuffer.get(), 0, _termLength);
}

}
