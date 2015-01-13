/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "IntFieldSource.h"
#include "_IntFieldSource.h"
#include "FieldCache.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

IntFieldSource::IntFieldSource(const String& field, const IntParserPtr& parser) : FieldCacheSource(field) {
    this->parser = parser;
}

IntFieldSource::~IntFieldSource() {
}

String IntFieldSource::description() {
    return L"int(" + FieldCacheSource::description() + L")";
}

DocValuesPtr IntFieldSource::getCachedFieldValues(const FieldCachePtr& cache, const String& field, const IndexReaderPtr& reader) {
    Collection<int32_t> arr(cache->getInts(reader, field, parser));
    return newLucene<IntDocValues>(shared_from_this(), arr);
}

bool IntFieldSource::cachedFieldSourceEquals(const FieldCacheSourcePtr& other) {
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    IntFieldSourcePtr otherSource(boost::dynamic_pointer_cast<IntFieldSource>(other));
    if (!otherSource) {
        return false;
    }
    return parser ? MiscUtils::equalTypes(parser, otherSource->parser) : !otherSource->parser;
}

int32_t IntFieldSource::cachedFieldSourceHashCode() {
    return StringUtils::hashCode(parser ? IntParser::_getClassName() : IntFieldSource::_getClassName());
}

IntDocValues::IntDocValues(const IntFieldSourcePtr& source, Collection<int32_t> arr) {
    this->_source = source;
    this->arr = arr;
}

IntDocValues::~IntDocValues() {
}

double IntDocValues::doubleVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return (double)arr[doc];
}

int32_t IntDocValues::intVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return arr[doc];
}

String IntDocValues::toString(int32_t doc) {
    return IntFieldSourcePtr(_source)->description() + L"=" + StringUtils::toString(intVal(doc));
}

CollectionValue IntDocValues::getInnerArray() {
    return arr;
}

}
