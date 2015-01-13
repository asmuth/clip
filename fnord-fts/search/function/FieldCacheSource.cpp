/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldCacheSource.h"
#include "FieldCache.h"
#include "StringUtils.h"

namespace Lucene {

FieldCacheSource::FieldCacheSource(const String& field) {
    this->field = field;
}

FieldCacheSource::~FieldCacheSource() {
}

DocValuesPtr FieldCacheSource::getValues(const IndexReaderPtr& reader) {
    return getCachedFieldValues(FieldCache::DEFAULT(), field, reader);
}

String FieldCacheSource::description() {
    return field;
}

bool FieldCacheSource::equals(const LuceneObjectPtr& other) {
    FieldCacheSourcePtr otherSource(boost::dynamic_pointer_cast<FieldCacheSource>(other));
    if (!otherSource) {
        return false;
    }
    return field == otherSource->field && cachedFieldSourceEquals(otherSource);
}

int32_t FieldCacheSource::hashCode() {
    return StringUtils::hashCode(field) + cachedFieldSourceHashCode();
}

}
