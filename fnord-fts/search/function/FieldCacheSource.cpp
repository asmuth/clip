/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/function/FieldCacheSource.h"
#include "fnord-fts/search/FieldCache.h"
#include "fnord-fts/util/StringUtils.h"

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
    FieldCacheSourcePtr otherSource(std::dynamic_pointer_cast<FieldCacheSource>(other));
    if (!otherSource) {
        return false;
    }
    return field == otherSource->field && cachedFieldSourceEquals(otherSource);
}

int32_t FieldCacheSource::hashCode() {
    return StringUtils::hashCode(field) + cachedFieldSourceHashCode();
}

}
