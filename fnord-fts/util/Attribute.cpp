/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Attribute.h"

namespace Lucene {

Attribute::~Attribute() {
}

int32_t Attribute::hashCode() {
    return LuceneObject::hashCode();
}

bool Attribute::equals(const LuceneObjectPtr& other) {
    return LuceneObject::equals(other);
}

LuceneObjectPtr Attribute::clone(const LuceneObjectPtr& other) {
    return LuceneObject::clone(other);
}

}
