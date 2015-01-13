/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NumericField.h"
#include "Field.h"
#include "NumericUtils.h"
#include "NumericTokenStream.h"
#include "StringUtils.h"

namespace Lucene {

NumericField::NumericField(const String& name)
    : AbstractField(name, Field::STORE_NO, Field::INDEX_ANALYZED_NO_NORMS, Field::TERM_VECTOR_NO) {
    setOmitTermFreqAndPositions(true);
    tokenStream = newLucene<NumericTokenStream>(NumericUtils::PRECISION_STEP_DEFAULT);
}

NumericField::NumericField(const String& name, Field::Store store, bool index)
    : AbstractField(name, store, index ? Field::INDEX_ANALYZED_NO_NORMS : Field::INDEX_NO, Field::TERM_VECTOR_NO) {
    setOmitTermFreqAndPositions(true);
    tokenStream = newLucene<NumericTokenStream>(NumericUtils::PRECISION_STEP_DEFAULT);
}

NumericField::NumericField(const String& name, int32_t precisionStep)
    : AbstractField(name, Field::STORE_NO, Field::INDEX_ANALYZED_NO_NORMS, Field::TERM_VECTOR_NO) {
    setOmitTermFreqAndPositions(true);
    tokenStream = newLucene<NumericTokenStream>(precisionStep);
}

NumericField::NumericField(const String& name, int32_t precisionStep, Field::Store store, bool index)
    : AbstractField(name, store, index ? Field::INDEX_ANALYZED_NO_NORMS : Field::INDEX_NO, Field::TERM_VECTOR_NO) {
    setOmitTermFreqAndPositions(true);
    tokenStream = newLucene<NumericTokenStream>(precisionStep);
}

NumericField::~NumericField() {
}

TokenStreamPtr NumericField::tokenStreamValue() {
    return isIndexed() ? boost::static_pointer_cast<TokenStream>(tokenStream) : TokenStreamPtr();
}

ByteArray NumericField::getBinaryValue(ByteArray result) {
    return ByteArray();
}

ReaderPtr NumericField::readerValue() {
    return ReaderPtr();
}

String NumericField::stringValue() {
    StringStream value;
    value << fieldsData;
    return value.str();
}

int64_t NumericField::getNumericValue() {
    return StringUtils::toLong(stringValue());
}

NumericFieldPtr NumericField::setLongValue(int64_t value) {
    tokenStream->setLongValue(value);
    fieldsData = value;
    return shared_from_this();
}

NumericFieldPtr NumericField::setIntValue(int32_t value) {
    tokenStream->setIntValue(value);
    fieldsData = value;
    return shared_from_this();
}

NumericFieldPtr NumericField::setDoubleValue(double value) {
    tokenStream->setDoubleValue(value);
    fieldsData = value;
    return shared_from_this();
}

}
