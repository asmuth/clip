/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Field.h"
#include "MiscUtils.h"
#include "StringUtils.h"
#include "VariantUtils.h"

namespace Lucene {

Field::Field(const String& name, const String& value, Store store, Index index) {
    ConstructField(name, value, store, index, TERM_VECTOR_NO);
}

Field::Field(const String& name, const String& value, Store store, Index index, TermVector termVector) {
    ConstructField(name, value, store, index, termVector);
}

Field::Field(const String& name, const ReaderPtr& reader) {
    ConstructField(name, reader, TERM_VECTOR_NO);
}

Field::Field(const String& name, const ReaderPtr& reader, TermVector termVector) {
    ConstructField(name, reader, termVector);
}

Field::Field(const String& name, const TokenStreamPtr& tokenStream) {
    ConstructField(name, tokenStream, TERM_VECTOR_NO);
}

Field::Field(const String& name, const TokenStreamPtr& tokenStream, TermVector termVector) {
    ConstructField(name, tokenStream, termVector);
}

Field::Field(const String& name, ByteArray value, Store store) {
    ConstructField(name, value, 0, value.size(), store);
}

Field::Field(const String& name, ByteArray value, int32_t offset, int32_t length, Store store) {
    ConstructField(name, value, offset, length, store);
}

void Field::ConstructField(const String& name, const String& value, Store store, Index index, TermVector termVector) {
    if (name.empty() && value.empty()) {
        boost::throw_exception(IllegalArgumentException(L"name and value cannot both be empty"));
    }
    if (index == INDEX_NO && store == STORE_NO) {
        boost::throw_exception(IllegalArgumentException(L"it doesn't make sense to have a field that is neither indexed nor stored"));
    }
    if (index == INDEX_NO && termVector != TERM_VECTOR_NO) {
        boost::throw_exception(IllegalArgumentException(L"cannot store term vector information for a field that is not indexed"));
    }

    this->_name = name;
    this->fieldsData = value;
    this->_isStored = isStored(store);
    this->_isIndexed = isIndexed(index);
    this->_isTokenized = isAnalyzed(index);
    this->_omitNorms = omitNorms(index);
    this->_isBinary = false;

    if (index == INDEX_NO) {
        this->omitTermFreqAndPositions = false;
    }

    setStoreTermVector(termVector);
}

void Field::ConstructField(const String& name, const ReaderPtr& reader, TermVector termVector) {
    this->_name = name;
    this->fieldsData = reader;
    this->_isStored = false;
    this->_isIndexed = true;
    this->_isTokenized = true;
    this->_isBinary = false;

    setStoreTermVector(termVector);
}

void Field::ConstructField(const String& name, const TokenStreamPtr& tokenStream, TermVector termVector) {
    this->_name = name;
    this->fieldsData = VariantUtils::null();
    this->tokenStream = tokenStream;
    this->_isStored = false;
    this->_isIndexed = true;
    this->_isTokenized = true;
    this->_isBinary = false;

    setStoreTermVector(termVector);
}

void Field::ConstructField(const String& name, ByteArray value, int32_t offset, int32_t length, Store store) {
    if (store == STORE_NO) {
        boost::throw_exception(IllegalArgumentException(L"binary values can't be unstored"));
    }

    this->_name = name;
    this->fieldsData = value;
    this->_isStored = isStored(store);
    this->_isIndexed = false;
    this->_isTokenized = false;
    this->omitTermFreqAndPositions = false;
    this->_omitNorms = true;
    this->_isBinary = true;
    this->binaryLength = length;
    this->binaryOffset = offset;

    setStoreTermVector(TERM_VECTOR_NO);
}

Field::~Field() {
}

String Field::stringValue() {
    return VariantUtils::get<String>(fieldsData);
}

ReaderPtr Field::readerValue() {
    return VariantUtils::get<ReaderPtr>(fieldsData);
}

TokenStreamPtr Field::tokenStreamValue() {
    return tokenStream;
}

void Field::setValue(const String& value) {
    if (_isBinary) {
        boost::throw_exception(IllegalArgumentException(L"cannot set a String value on a binary field"));
    }
    fieldsData = value;
}

void Field::setValue(const ReaderPtr& value) {
    if (_isBinary) {
        boost::throw_exception(IllegalArgumentException(L"cannot set a Reader value on a binary field"));
    }
    if (_isStored) {
        boost::throw_exception(IllegalArgumentException(L"cannot set a Reader value on a stored field"));
    }
    fieldsData = value;
}

void Field::setValue(ByteArray value) {
    if (!_isBinary) {
        boost::throw_exception(IllegalArgumentException(L"cannot set a byte[] value on a non-binary field"));
    }
    fieldsData = value;
    binaryLength = value.size();
    binaryOffset = 0;
}

void Field::setValue(ByteArray value, int32_t offset, int32_t length) {
    if (!_isBinary) {
        boost::throw_exception(IllegalArgumentException(L"cannot set a byte[] value on a non-binary field"));
    }
    fieldsData = value;
    binaryLength = length;
    binaryOffset = offset;
}

void Field::setTokenStream(const TokenStreamPtr& tokenStream) {
    this->_isIndexed = true;
    this->_isTokenized = true;
    this->tokenStream = tokenStream;
}

bool Field::isStored(Store store) {
    switch (store) {
    case STORE_YES:
        return true;

    case STORE_NO:
        return false;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field store"));
        return false;
    }
}

bool Field::isIndexed(Index index) {
    switch (index) {
    case INDEX_NO:
        return false;

    case INDEX_ANALYZED:
        return true;

    case INDEX_NOT_ANALYZED:
        return true;

    case INDEX_NOT_ANALYZED_NO_NORMS:
        return true;

    case INDEX_ANALYZED_NO_NORMS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field index"));
        return false;
    }
}

bool Field::isAnalyzed(Index index) {
    switch (index) {
    case INDEX_NO:
        return false;

    case INDEX_ANALYZED:
        return true;

    case INDEX_NOT_ANALYZED:
        return false;

    case INDEX_NOT_ANALYZED_NO_NORMS:
        return false;

    case INDEX_ANALYZED_NO_NORMS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field index"));
        return false;
    }
}

bool Field::omitNorms(Index index) {
    switch (index) {
    case INDEX_NO:
        return true;

    case INDEX_ANALYZED:
        return false;

    case INDEX_NOT_ANALYZED:
        return false;

    case INDEX_NOT_ANALYZED_NO_NORMS:
        return true;

    case INDEX_ANALYZED_NO_NORMS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field index"));
        return false;
    }
}

Field::Index Field::toIndex(bool indexed, bool analyzed) {
    return toIndex(indexed, analyzed, false);
}

Field::Index Field::toIndex(bool indexed, bool analyzed, bool omitNorms) {
    // If it is not indexed nothing else matters
    if (!indexed) {
        return INDEX_NO;
    }

    // typical, non-expert
    if (!omitNorms) {
        return analyzed ? INDEX_ANALYZED : INDEX_NOT_ANALYZED;
    }

    // Expert: Norms omitted
    return analyzed ? INDEX_ANALYZED_NO_NORMS : INDEX_NOT_ANALYZED_NO_NORMS;
}

bool Field::isStored(TermVector termVector) {
    switch (termVector) {
    case TERM_VECTOR_NO:
        return false;

    case TERM_VECTOR_YES:
        return true;

    case TERM_VECTOR_WITH_POSITIONS:
        return true;

    case TERM_VECTOR_WITH_OFFSETS:
        return true;

    case TERM_VECTOR_WITH_POSITIONS_OFFSETS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field term vector"));
        return false;
    }
}

bool Field::withPositions(TermVector termVector) {
    switch (termVector) {
    case TERM_VECTOR_NO:
        return false;

    case TERM_VECTOR_YES:
        return false;

    case TERM_VECTOR_WITH_POSITIONS:
        return true;

    case TERM_VECTOR_WITH_OFFSETS:
        return false;

    case TERM_VECTOR_WITH_POSITIONS_OFFSETS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field term vector"));
        return false;
    }
}

bool Field::withOffsets(TermVector termVector) {
    switch (termVector) {
    case TERM_VECTOR_NO:
        return false;

    case TERM_VECTOR_YES:
        return false;

    case TERM_VECTOR_WITH_POSITIONS:
        return false;

    case TERM_VECTOR_WITH_OFFSETS:
        return true;

    case TERM_VECTOR_WITH_POSITIONS_OFFSETS:
        return true;

    default:
        boost::throw_exception(IllegalArgumentException(L"Invalid field term vector"));
        return false;
    }
}

Field::TermVector Field::toTermVector(bool stored, bool withOffsets, bool withPositions) {
    // If it is not stored, nothing else matters.
    if (!stored) {
        return TERM_VECTOR_NO;
    }

    if (withOffsets) {
        return withPositions ? TERM_VECTOR_WITH_POSITIONS_OFFSETS : TERM_VECTOR_WITH_OFFSETS;
    }

    return withPositions ? TERM_VECTOR_WITH_POSITIONS : TERM_VECTOR_YES;
}

}
