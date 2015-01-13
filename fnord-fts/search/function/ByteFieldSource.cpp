/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ByteFieldSource.h"
#include "_ByteFieldSource.h"
#include "FieldCache.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

ByteFieldSource::ByteFieldSource(const String& field, const ByteParserPtr& parser) : FieldCacheSource(field) {
    this->parser = parser;
}

ByteFieldSource::~ByteFieldSource() {
}

String ByteFieldSource::description() {
    return L"byte(" + FieldCacheSource::description() + L")";
}

DocValuesPtr ByteFieldSource::getCachedFieldValues(const FieldCachePtr& cache, const String& field, const IndexReaderPtr& reader) {
    Collection<uint8_t> arr(cache->getBytes(reader, field, parser));
    return newLucene<ByteDocValues>(shared_from_this(), arr);
}

bool ByteFieldSource::cachedFieldSourceEquals(const FieldCacheSourcePtr& other) {
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    ByteFieldSourcePtr otherSource(boost::dynamic_pointer_cast<ByteFieldSource>(other));
    if (!otherSource) {
        return false;
    }
    return parser ? MiscUtils::equalTypes(parser, otherSource->parser) : !otherSource->parser;
}

int32_t ByteFieldSource::cachedFieldSourceHashCode() {
    return StringUtils::hashCode(parser ? ByteParser::_getClassName() : ByteFieldSource::_getClassName());
}

ByteDocValues::ByteDocValues(const ByteFieldSourcePtr& source, Collection<uint8_t> arr) {
    this->_source = source;
    this->arr = arr;
}

ByteDocValues::~ByteDocValues() {
}

double ByteDocValues::doubleVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return (double)arr[doc];
}

int32_t ByteDocValues::intVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return (int32_t)arr[doc];
}

String ByteDocValues::toString(int32_t doc) {
    return ByteFieldSourcePtr(_source)->description() + L"=" + StringUtils::toString(intVal(doc));
}

CollectionValue ByteDocValues::getInnerArray() {
    return arr;
}

}
