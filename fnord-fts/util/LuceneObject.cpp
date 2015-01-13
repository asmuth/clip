/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LuceneObject.h"
#include "StringUtils.h"

namespace Lucene {

LuceneObject::LuceneObject() {
}

LuceneObject::~LuceneObject() {
}

void LuceneObject::initialize() {
    // override
}

LuceneObjectPtr LuceneObject::clone(const LuceneObjectPtr& other) {
    if (!other) {
        boost::throw_exception(UnsupportedOperationException(L"clone must not be null"));
    }
    return other;
}

int32_t LuceneObject::hashCode() {
    return (int32_t)(int64_t)this;
}

bool LuceneObject::equals(const LuceneObjectPtr& other) {
    return (other && this == other.get());
}

int32_t LuceneObject::compareTo(const LuceneObjectPtr& other) {
    return (int32_t)(this - other.get());
}

String LuceneObject::toString() {
    return StringUtils::toString(hashCode());
}

}
