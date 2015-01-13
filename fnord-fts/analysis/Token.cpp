/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Token.h"
#include "Payload.h"
#include "TermAttribute.h"
#include "OffsetAttribute.h"
#include "PositionIncrementAttribute.h"
#include "PayloadAttribute.h"
#include "FlagsAttribute.h"
#include "TypeAttribute.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t Token::MIN_BUFFER_SIZE = 10;

Token::Token() {
    ConstructToken(0, 0, DEFAULT_TYPE(), 0);
}

Token::Token(int32_t start, int32_t end) {
    ConstructToken(start, end, DEFAULT_TYPE(), 0);
}

Token::Token(int32_t start, int32_t end, const String& type) {
    ConstructToken(start, end, type, 0);
}

Token::Token(int32_t start, int32_t end, int32_t flags) {
    ConstructToken(start, end, DEFAULT_TYPE(), flags);
}

Token::Token(const String& text, int32_t start, int32_t end) {
    ConstructToken(start, end, DEFAULT_TYPE(), 0);
    setTermBuffer(text);
}

Token::Token(const String& text, int32_t start, int32_t end, const String& type) {
    ConstructToken(start, end, type, 0);
    setTermBuffer(text);
}

Token::Token(const String& text, int32_t start, int32_t end, int32_t flags) {
    ConstructToken(start, end, DEFAULT_TYPE(), flags);
    setTermBuffer(text);
}

Token::Token(CharArray startTermBuffer, int32_t termBufferOffset, int32_t termBufferLength, int32_t start, int32_t end) {
    ConstructToken(start, end, DEFAULT_TYPE(), 0);
    setTermBuffer(startTermBuffer.get(), termBufferOffset, termBufferLength);
}

Token::~Token() {
}

void Token::ConstructToken(int32_t start, int32_t end, const String& type, int32_t flags) {
    this->_termLength = 0;
    this->_startOffset = start;
    this->_endOffset = end;
    this->_type = type;
    this->flags = flags;
    this->positionIncrement = 1;
}

const String& Token::DEFAULT_TYPE() {
    static String _DEFAULT_TYPE(L"word");
    return _DEFAULT_TYPE;
}

void Token::setPositionIncrement(int32_t positionIncrement) {
    if (positionIncrement < 0) {
        boost::throw_exception(IllegalArgumentException(L"Increment must be zero or greater: " + StringUtils::toString(positionIncrement)));
    }
    this->positionIncrement = positionIncrement;
}

int32_t Token::getPositionIncrement() {
    return positionIncrement;
}

String Token::term() {
    initTermBuffer();
    return String(_termBuffer.get(), _termLength);
}

void Token::setTermBuffer(const wchar_t* buffer, int32_t offset, int32_t length) {
    growTermBuffer(length);
    MiscUtils::arrayCopy(buffer, offset, _termBuffer.get(), 0, length);
    _termLength = length;
}

void Token::setTermBuffer(const String& buffer) {
    int32_t length = (int32_t)buffer.size();
    growTermBuffer(length);
    MiscUtils::arrayCopy(buffer.begin(), 0, _termBuffer.get(), 0, length);
    _termLength = length;
}

void Token::setTermBuffer(const String& buffer, int32_t offset, int32_t length) {
    BOOST_ASSERT(offset <= (int32_t)buffer.length());
    BOOST_ASSERT(offset + length <= (int32_t)buffer.length());
    growTermBuffer(length);
    MiscUtils::arrayCopy(buffer.begin(), offset, _termBuffer.get(), 0, length);
    _termLength = length;
}

CharArray Token::termBuffer() {
    if (!_termBuffer) {
        initTermBuffer();
    }
    return _termBuffer;
}

wchar_t* Token::termBufferArray() {
    if (!_termBuffer) {
        initTermBuffer();
    }
    return _termBuffer.get();
}

CharArray Token::resizeTermBuffer(int32_t newSize) {
    if (!_termBuffer) {
        // The buffer is always at least MIN_BUFFER_SIZE
        _termBuffer = CharArray::newInstance(MiscUtils::getNextSize(std::max(newSize, MIN_BUFFER_SIZE)));
    } else {
        if (_termBuffer.size() < newSize) {
            // Not big enough; create a new array with slight over allocation and preserve content
            _termBuffer.resize(MiscUtils::getNextSize(newSize));
        }
    }
    return _termBuffer;
}

void Token::growTermBuffer(int32_t newSize) {
    _termBuffer = resizeTermBuffer(newSize);
}

void Token::initTermBuffer() {
    if (!_termBuffer) {
        _termBuffer = CharArray::newInstance(MiscUtils::getNextSize(MIN_BUFFER_SIZE));
        _termLength = 0;
    }
}

int32_t Token::termLength() {
    if (!_termBuffer) {
        initTermBuffer();
    }
    return _termLength;
}

void Token::setTermLength(int32_t length) {
    initTermBuffer();
    if (length > _termBuffer.size()) {
        boost::throw_exception(IllegalArgumentException(L"length " + StringUtils::toString(length) +
                               L" exceeds the size of the termBuffer (" +
                               StringUtils::toString(_termBuffer.size()) + L")"));
    }
    _termLength = length;
}

int32_t Token::startOffset() {
    return _startOffset;
}

void Token::setStartOffset(int32_t offset) {
    this->_startOffset = offset;
}

int32_t Token::endOffset() {
    return _endOffset;
}

void Token::setEndOffset(int32_t offset) {
    this->_endOffset = offset;
}

void Token::setOffset(int32_t startOffset, int32_t endOffset) {
    this->_startOffset = startOffset;
    this->_endOffset = endOffset;
}

String Token::type() {
    return _type;
}

void Token::setType(const String& type) {
    this->_type = type;
}

int32_t Token::getFlags() {
    return flags;
}

void Token::setFlags(int32_t flags) {
    this->flags = flags;
}

PayloadPtr Token::getPayload() {
    return this->payload;
}

void Token::setPayload(const PayloadPtr& payload) {
    this->payload = payload;
}

String Token::toString() {
    StringStream buffer;
    initTermBuffer();
    buffer << L"(";
    if (!_termBuffer) {
        buffer << L"null";
    } else {
        buffer << term() << L"," << _startOffset << L"," << _endOffset;
    }
    if (_type != L"word") {
        buffer << L",type=" << _type;
    }
    if (positionIncrement != 1) {
        buffer << L",posIncr=" << positionIncrement;
    }
    buffer << L")";
    return buffer.str();
}

void Token::clear() {
    payload.reset();
    // Leave termBuffer to allow re-use
    _termLength = 0;
    positionIncrement = 1;
    flags = 0;
    _startOffset = 0;
    _endOffset = 0;
    _type = DEFAULT_TYPE();
}

LuceneObjectPtr Token::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Attribute::clone(other ? other : newLucene<Token>());
    TokenPtr cloneToken(boost::dynamic_pointer_cast<Token>(clone));
    cloneToken->_termLength = _termLength;
    cloneToken->_startOffset = _startOffset;
    cloneToken->_endOffset = _endOffset;
    cloneToken->_type = _type;
    cloneToken->flags = flags;
    cloneToken->positionIncrement = positionIncrement;

    // Do a deep clone
    if (_termBuffer) {
        cloneToken->_termBuffer = CharArray::newInstance(_termBuffer.size());
        MiscUtils::arrayCopy(_termBuffer.get(), 0, cloneToken->_termBuffer.get(), 0, _termBuffer.size());
    }
    if (payload) {
        cloneToken->payload = boost::dynamic_pointer_cast<Payload>(payload->clone());
    }

    return cloneToken;
}

TokenPtr Token::clone(CharArray newTermBuffer, int32_t newTermOffset, int32_t newTermLength, int32_t newStartOffset, int32_t newEndOffset) {
    TokenPtr clone(newLucene<Token>(newTermBuffer, newTermOffset, newTermLength, newStartOffset, newEndOffset));
    clone->positionIncrement = positionIncrement;
    clone->flags = flags;
    clone->_type = _type;
    if (payload) {
        clone->payload = boost::dynamic_pointer_cast<Payload>(payload->clone());
    }
    return clone;
}

bool Token::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    TokenPtr otherToken(boost::dynamic_pointer_cast<Token>(other));
    if (otherToken) {
        initTermBuffer();
        otherToken->initTermBuffer();

        if (_termLength == otherToken->_termLength && _startOffset == otherToken->_startOffset &&
                _endOffset == otherToken->_endOffset && flags == otherToken->flags &&
                positionIncrement == otherToken->positionIncrement && _type == otherToken->_type &&
                (payload ? payload->equals(otherToken->payload) : !otherToken->payload)) {
            for (int32_t i = 0; i < _termLength; ++i) {
                if (_termBuffer[i] != otherToken->_termBuffer[i]) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

int32_t Token::hashCode() {
    initTermBuffer();
    int32_t code = _termLength;
    code = code * 31 + _startOffset;
    code = code * 31 + _endOffset;
    code = code * 31 + flags;
    code = code * 31 + positionIncrement;
    code = code * 31 + StringUtils::hashCode(_type);
    code = payload ? code * 31 + payload->hashCode() : code;
    code = code * 31 + MiscUtils::hashCode(_termBuffer.get(), 0, _termLength);
    return code;
}

void Token::clearNoTermBuffer() {
    payload.reset();
    positionIncrement = 1;
    flags = 0;
    _startOffset = 0;
    _endOffset = 0;
    _type = DEFAULT_TYPE();
}

TokenPtr Token::reinit(CharArray newTermBuffer, int32_t newTermOffset, int32_t newTermLength, int32_t newStartOffset, int32_t newEndOffset, const String& newType) {
    clearNoTermBuffer();
    payload.reset();
    positionIncrement = 1;
    setTermBuffer(newTermBuffer.get(), newTermOffset, newTermLength);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = newType;
    return shared_from_this();
}

TokenPtr Token::reinit(CharArray newTermBuffer, int32_t newTermOffset, int32_t newTermLength, int32_t newStartOffset, int32_t newEndOffset) {
    clearNoTermBuffer();
    setTermBuffer(newTermBuffer.get(), newTermOffset, newTermLength);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = DEFAULT_TYPE();
    return shared_from_this();
}

TokenPtr Token::reinit(const String& newTerm, int32_t newStartOffset, int32_t newEndOffset, const String& newType) {
    clearNoTermBuffer();
    setTermBuffer(newTerm);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = newType;
    return shared_from_this();
}

TokenPtr Token::reinit(const String& newTerm, int32_t newTermOffset, int32_t newTermLength, int32_t newStartOffset, int32_t newEndOffset, const String& newType) {
    clearNoTermBuffer();
    setTermBuffer(newTerm, newTermOffset, newTermLength);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = newType;
    return shared_from_this();
}

TokenPtr Token::reinit(const String& newTerm, int32_t newStartOffset, int32_t newEndOffset) {
    clearNoTermBuffer();
    setTermBuffer(newTerm);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = DEFAULT_TYPE();
    return shared_from_this();
}

TokenPtr Token::reinit(const String& newTerm, int32_t newTermOffset, int32_t newTermLength, int32_t newStartOffset, int32_t newEndOffset) {
    clearNoTermBuffer();
    setTermBuffer(newTerm, newTermOffset, newTermLength);
    _startOffset = newStartOffset;
    _endOffset = newEndOffset;
    _type = DEFAULT_TYPE();
    return shared_from_this();
}

void Token::reinit(const TokenPtr& prototype) {
    prototype->initTermBuffer();
    setTermBuffer(prototype->_termBuffer.get(), 0, prototype->_termLength);
    positionIncrement = prototype->positionIncrement;
    flags = prototype->flags;
    _startOffset = prototype->_startOffset;
    _endOffset = prototype->_endOffset;
    _type = prototype->_type;
    payload = prototype->payload;
}

void Token::reinit(const TokenPtr& prototype, const String& newTerm) {
    setTermBuffer(newTerm);
    positionIncrement = prototype->positionIncrement;
    flags = prototype->flags;
    _startOffset = prototype->_startOffset;
    _endOffset = prototype->_endOffset;
    _type = prototype->_type;
    payload = prototype->payload;
}

void Token::reinit(const TokenPtr& prototype, CharArray newTermBuffer, int32_t offset, int32_t length) {
    setTermBuffer(newTermBuffer.get(), offset, length);
    positionIncrement = prototype->positionIncrement;
    flags = prototype->flags;
    _startOffset = prototype->_startOffset;
    _endOffset = prototype->_endOffset;
    _type = prototype->_type;
    payload = prototype->payload;
}

void Token::copyTo(const AttributePtr& target) {
    TokenPtr targetToken(boost::dynamic_pointer_cast<Token>(target));
    if (targetToken) {
        targetToken->reinit(shared_from_this());
        // reinit shares the payload, so clone it
        if (payload) {
            targetToken->payload = boost::dynamic_pointer_cast<Payload>(payload->clone());
        }
    } else {
        initTermBuffer();
        TermAttributePtr targetTermAttribute(boost::dynamic_pointer_cast<TermAttribute>(target));
        if (targetTermAttribute) {
            targetTermAttribute->setTermBuffer(_termBuffer.get(), 0, _termLength);
        }
        OffsetAttributePtr targetOffsetAttribute(boost::dynamic_pointer_cast<OffsetAttribute>(target));
        if (targetOffsetAttribute) {
            targetOffsetAttribute->setOffset(_startOffset, _endOffset);
        }
        PositionIncrementAttributePtr targetPositionIncrementAttribute(boost::dynamic_pointer_cast<PositionIncrementAttribute>(target));
        if (targetPositionIncrementAttribute) {
            targetPositionIncrementAttribute->setPositionIncrement(positionIncrement);
        }
        PayloadAttributePtr targetPayloadAttribute(boost::dynamic_pointer_cast<PayloadAttribute>(target));
        if (targetPayloadAttribute) {
            targetPayloadAttribute->setPayload(payload ? boost::dynamic_pointer_cast<Payload>(payload->clone()) : PayloadPtr());
        }
        FlagsAttributePtr targetFlagsAttribute(boost::dynamic_pointer_cast<FlagsAttribute>(target));
        if (targetFlagsAttribute) {
            targetFlagsAttribute->setFlags(flags);
        }
        TypeAttributePtr targetTypeAttribute(boost::dynamic_pointer_cast<TypeAttribute>(target));
        if (targetTypeAttribute) {
            targetTypeAttribute->setType(_type);
        }
    }
}

AttributeFactoryPtr Token::TOKEN_ATTRIBUTE_FACTORY() {
    static AttributeFactoryPtr _TOKEN_ATTRIBUTE_FACTORY;
    if (!_TOKEN_ATTRIBUTE_FACTORY) {
        _TOKEN_ATTRIBUTE_FACTORY = newLucene<TokenAttributeFactory>(AttributeFactory::DEFAULT_ATTRIBUTE_FACTORY());
        CycleCheck::addStatic(_TOKEN_ATTRIBUTE_FACTORY);
    }
    return _TOKEN_ATTRIBUTE_FACTORY;
}

TokenAttributeFactory::TokenAttributeFactory(const AttributeFactoryPtr& delegate) {
    this->delegate = delegate;
}

TokenAttributeFactory::~TokenAttributeFactory() {
}

AttributePtr TokenAttributeFactory::createAttributeInstance(const String& className) {
    return newLucene<Token>();
}

bool TokenAttributeFactory::equals(const LuceneObjectPtr& other) {
    if (AttributeFactory::equals(other)) {
        return true;
    }

    TokenAttributeFactoryPtr otherTokenAttributeFactory(boost::dynamic_pointer_cast<TokenAttributeFactory>(other));
    if (otherTokenAttributeFactory) {
        return this->delegate->equals(otherTokenAttributeFactory->delegate);
    }
    return false;
}

int32_t TokenAttributeFactory::hashCode() {
    return (delegate->hashCode() ^ 0x0a45aa31);
}

}
