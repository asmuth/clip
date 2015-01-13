/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ReverseOrdFieldSource.h"
#include "_ReverseOrdFieldSource.h"
#include "FieldCache.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

ReverseOrdFieldSource::ReverseOrdFieldSource(const String& field) {
    this->field = field;
}

ReverseOrdFieldSource::~ReverseOrdFieldSource() {
}

String ReverseOrdFieldSource::description() {
    return L"rord(" + field + L")";
}

DocValuesPtr ReverseOrdFieldSource::getValues(const IndexReaderPtr& reader) {
    StringIndexPtr sindex(FieldCache::DEFAULT()->getStringIndex(reader, field));
    Collection<int32_t> arr(sindex->order);
    int32_t end = sindex->lookup.size();
    return newLucene<ReverseOrdDocValues>(shared_from_this(), arr, end);
}

bool ReverseOrdFieldSource::equals(const LuceneObjectPtr& other) {
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    ReverseOrdFieldSourcePtr otherSource(boost::dynamic_pointer_cast<ReverseOrdFieldSource>(other));
    if (!otherSource) {
        return false;
    }
    return field == otherSource->field;
}

int32_t ReverseOrdFieldSource::hashCode() {
    return StringUtils::hashCode(ReverseOrdFieldSource::_getClassName()) + StringUtils::hashCode(field);
}

ReverseOrdDocValues::ReverseOrdDocValues(const ReverseOrdFieldSourcePtr& source, Collection<int32_t> arr, int32_t end) {
    this->_source = source;
    this->arr = arr;
    this->end = end;
}

ReverseOrdDocValues::~ReverseOrdDocValues() {
}

double ReverseOrdDocValues::doubleVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return (double)(end - arr[doc]);
}

int32_t ReverseOrdDocValues::intVal(int32_t doc) {
    if (doc < 0 || doc >= arr.size()) {
        boost::throw_exception(IndexOutOfBoundsException());
    }
    return (end - arr[doc]);
}

String ReverseOrdDocValues::strVal(int32_t doc) {
    // the string value of the ordinal, not the string itself
    return StringUtils::toString(intVal(doc));
}

String ReverseOrdDocValues::toString(int32_t doc) {
    return ReverseOrdFieldSourcePtr(_source)->description() + L"=" + strVal(doc);
}

CollectionValue ReverseOrdDocValues::getInnerArray() {
    return arr;
}

}
