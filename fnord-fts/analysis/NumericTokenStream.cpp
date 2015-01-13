/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NumericTokenStream.h"
#include "NumericUtils.h"
#include "AttributeSource.h"
#include "TermAttribute.h"
#include "TypeAttribute.h"
#include "PositionIncrementAttribute.h"

namespace Lucene {

NumericTokenStream::NumericTokenStream() {
    this->shift = 0;
    this->valSize = 0;
    this->termAtt = addAttribute<TermAttribute>();
    this->typeAtt = addAttribute<TypeAttribute>();
    this->posIncrAtt = addAttribute<PositionIncrementAttribute>();
    this->precisionStep = NumericUtils::PRECISION_STEP_DEFAULT;
}

NumericTokenStream::NumericTokenStream(int32_t precisionStep) {
    this->shift = 0;
    this->valSize = 0;
    this->termAtt = addAttribute<TermAttribute>();
    this->typeAtt = addAttribute<TypeAttribute>();
    this->posIncrAtt = addAttribute<PositionIncrementAttribute>();
    this->precisionStep = precisionStep;
    if (precisionStep < 1) {
        boost::throw_exception(IllegalArgumentException(L"precisionStep must be >=1"));
    }
}

NumericTokenStream::NumericTokenStream(const AttributeSourcePtr& source, int32_t precisionStep) : TokenStream(source) {
    this->shift = 0;
    this->valSize = 0;
    this->termAtt = addAttribute<TermAttribute>();
    this->typeAtt = addAttribute<TypeAttribute>();
    this->posIncrAtt = addAttribute<PositionIncrementAttribute>();
    this->precisionStep = precisionStep;
    if (precisionStep < 1) {
        boost::throw_exception(IllegalArgumentException(L"precisionStep must be >=1"));
    }
}

NumericTokenStream::NumericTokenStream(const AttributeFactoryPtr& factory, int32_t precisionStep) : TokenStream(factory) {
    this->shift = 0;
    this->valSize = 0;
    this->termAtt = addAttribute<TermAttribute>();
    this->typeAtt = addAttribute<TypeAttribute>();
    this->posIncrAtt = addAttribute<PositionIncrementAttribute>();
    this->precisionStep = precisionStep;
    if (precisionStep < 1) {
        boost::throw_exception(IllegalArgumentException(L"precisionStep must be >=1"));
    }
}

NumericTokenStream::~NumericTokenStream() {
}

const String& NumericTokenStream::TOKEN_TYPE_FULL_PREC() {
    static String _TOKEN_TYPE_FULL_PREC(L"fullPrecNumeric");
    return _TOKEN_TYPE_FULL_PREC;
}

const String& NumericTokenStream::TOKEN_TYPE_LOWER_PREC() {
    static String _TOKEN_TYPE_LOWER_PREC(L"lowerPrecNumeric");
    return _TOKEN_TYPE_LOWER_PREC;
}

NumericTokenStreamPtr NumericTokenStream::setLongValue(int64_t value) {
    this->value = value;
    valSize = 64;
    shift = 0;
    return shared_from_this();
}

NumericTokenStreamPtr NumericTokenStream::setIntValue(int32_t value) {
    this->value = (int64_t)value;
    valSize = 32;
    shift = 0;
    return shared_from_this();
}

NumericTokenStreamPtr NumericTokenStream::setDoubleValue(double value) {
    this->value = NumericUtils::doubleToSortableLong(value);
    valSize = 64;
    shift = 0;
    return shared_from_this();
}

void NumericTokenStream::reset() {
    if (valSize == 0) {
        boost::throw_exception(IllegalStateException(L"call setValue() before usage"));
    }
    shift = 0;
}

bool NumericTokenStream::incrementToken() {
    if (valSize == 0) {
        boost::throw_exception(IllegalStateException(L"call setValue() before usage"));
    }
    if (shift >= valSize) {
        return false;
    }

    clearAttributes();
    CharArray buffer;
    switch (valSize) {
    case 64:
        buffer = termAtt->resizeTermBuffer(NumericUtils::BUF_SIZE_LONG);
        termAtt->setTermLength(NumericUtils::longToPrefixCoded(value, shift, buffer));
        break;
    case 32:
        buffer = termAtt->resizeTermBuffer(NumericUtils::BUF_SIZE_INT);
        termAtt->setTermLength(NumericUtils::intToPrefixCoded((int32_t)value, shift, buffer));
        break;
    default:
        // should not happen
        boost::throw_exception(IllegalArgumentException(L"valSize must be 32 or 64"));
    }

    typeAtt->setType(shift == 0 ? TOKEN_TYPE_FULL_PREC() : TOKEN_TYPE_LOWER_PREC());
    posIncrAtt->setPositionIncrement(shift == 0 ? 1 : 0);
    shift += precisionStep;
    return true;
}

String NumericTokenStream::toString() {
    StringStream buffer;
    buffer << L"(numeric,valSize=" << valSize << L",precisionStep=" << precisionStep << L")";
    return buffer.str();
}

}
