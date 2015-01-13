/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DoubleFieldSource.h"
#include "FieldCache.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

DoubleFieldSource::DoubleFieldSource(const String& field, const DoubleParserPtr& parser) : FieldCacheSource(field) {
    this->parser = parser;
}

DoubleFieldSource::~DoubleFieldSource() {
}

String DoubleFieldSource::description() {
    return L"double(" + FieldCacheSource::description() + L")";
}

DocValuesPtr DoubleFieldSource::getCachedFieldValues(const FieldCachePtr& cache, const String& field, const IndexReaderPtr& reader) {
    Collection<double> arr(cache->getDoubles(reader, field, parser));
    return newLucene<DoubleDocValues>(shared_from_this(), arr);
}

bool DoubleFieldSource::cachedFieldSourceEquals(const FieldCacheSourcePtr& other) {
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    DoubleFieldSourcePtr otherSource(boost::dynamic_pointer_cast<DoubleFieldSource>(other));
    if (!otherSource) {
        return false;
    }
    return parser ? MiscUtils::equalTypes(parser, otherSource->parser) : !otherSource->parser;
}

int32_t DoubleFieldSource::cachedFieldSourceHashCode() {
    return StringUtils::hashCode(parser ? DoubleParser::_getClassName() : DoubleFieldSource::_getClassName());
}

DoubleDocValues::DoubleDocValues(const DoubleFieldSourcePtr& source, Collection<double> arr) {
    this->_source = source;
    this->arr = arr;
}

DoubleDocValues::~DoubleDocValues() {
}

double DoubleDocValues::doubleVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return arr[doc];
}

String DoubleDocValues::toString(int32_t doc) {
    return DoubleFieldSourcePtr(_source)->description() + L"=" + StringUtils::toString(doubleVal(doc));
}

CollectionValue DoubleDocValues::getInnerArray() {
    return arr;
}

}
