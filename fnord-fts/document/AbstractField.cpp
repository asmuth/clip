/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "AbstractField.h"
#include "Field.h"
#include "StringUtils.h"
#include "VariantUtils.h"

namespace Lucene {

AbstractField::AbstractField() {
    this->_name = L"body";
    this->storeTermVector = false;
    this->storeOffsetWithTermVector = false;
    this->storePositionWithTermVector = false;
    this->_omitNorms = false;
    this->_isStored = false;
    this->_isIndexed = true;
    this->_isTokenized = true;
    this->_isBinary = false;

    this->lazy = false;
    this->omitTermFreqAndPositions = false;
    this->boost = 1.0;
    this->fieldsData = VariantUtils::null();

    this->binaryLength = 0;
    this->binaryOffset = 0;
}

AbstractField::AbstractField(const String& name, Field::Store store, Field::Index index, Field::TermVector termVector) {
    this->_name = name;
    this->_isStored = Field::isStored(store);
    this->_isIndexed = Field::isIndexed(index);
    this->_isTokenized = Field::isAnalyzed(index);
    this->_omitNorms = Field::omitNorms(index);
    this->_isBinary = false;

    this->lazy = false;
    this->omitTermFreqAndPositions = false;
    this->boost = 1.0;
    this->fieldsData = VariantUtils::null();

    this->binaryLength = 0;
    this->binaryOffset = 0;

    setStoreTermVector(termVector);
}

AbstractField::~AbstractField() {
}

void AbstractField::setBoost(double boost) {
    this->boost = boost;
}

double AbstractField::getBoost() {
    return boost;
}

String AbstractField::name() {
    return _name;
}

void AbstractField::setStoreTermVector(Field::TermVector termVector) {
    this->storeTermVector = Field::isStored(termVector);
    this->storePositionWithTermVector = Field::withPositions(termVector);
    this->storeOffsetWithTermVector = Field::withOffsets(termVector);
}

bool AbstractField::isStored() {
    return _isStored;
}

bool AbstractField::isIndexed() {
    return _isIndexed;
}

bool AbstractField::isTokenized() {
    return _isTokenized;
}

bool AbstractField::isTermVectorStored() {
    return storeTermVector;
}

bool AbstractField::isStoreOffsetWithTermVector() {
    return storeOffsetWithTermVector;
}

bool AbstractField::isStorePositionWithTermVector() {
    return storePositionWithTermVector;
}

bool AbstractField::isBinary() {
    return _isBinary;
}

ByteArray AbstractField::getBinaryValue() {
    return getBinaryValue(ByteArray());
}

ByteArray AbstractField::getBinaryValue(ByteArray result) {
    return VariantUtils::get<ByteArray>(fieldsData);
}

int32_t AbstractField::getBinaryLength() {
    if (_isBinary) {
        return binaryLength;
    }
    ByteArray binary(VariantUtils::get<ByteArray>(fieldsData));
    return binary ? binary.size() : 0;
}

int32_t AbstractField::getBinaryOffset() {
    return binaryOffset;
}

bool AbstractField::getOmitNorms() {
    return _omitNorms;
}

bool AbstractField::getOmitTermFreqAndPositions() {
    return omitTermFreqAndPositions;
}

void AbstractField::setOmitNorms(bool omitNorms) {
    this->_omitNorms = omitNorms;
}

void AbstractField::setOmitTermFreqAndPositions(bool omitTermFreqAndPositions) {
    this->omitTermFreqAndPositions = omitTermFreqAndPositions;
}

bool AbstractField::isLazy() {
    return lazy;
}

String AbstractField::toString() {
    StringStream result;
    if (_isStored) {
        result << L"stored";
    }
    if (_isIndexed) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"indexed";
    }
    if (_isTokenized) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"tokenized";
    }
    if (storeTermVector) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"termVector";
    }
    if (storeOffsetWithTermVector) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"termVectorOffsets";
    }
    if (storePositionWithTermVector) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"termVectorPosition";
    }
    if (_isBinary) {
        if (!result.str().empty()) {
            result << L",";
        }
        result << L"binary";
    }
    if (_omitNorms) {
        result << L",omitNorms";
    }
    if (omitTermFreqAndPositions) {
        result << L",omitTermFreqAndPositions";
    }
    if (lazy) {
        result << L",lazy";
    }
    result << L"<" << _name << L":";

    if (VariantUtils::typeOf<String>(fieldsData)) {
        result << VariantUtils::get<String>(fieldsData);
    } else if (VariantUtils::typeOf<ReaderPtr>(fieldsData)) {
        result << L"Reader";
    } else if (VariantUtils::typeOf<ByteArray>(fieldsData)) {
        result << L"Binary [size=" << StringUtils::toString(VariantUtils::get<ByteArray>(fieldsData).size()) << L"]";
    }

    result << L">";
    return result.str();
}

}
