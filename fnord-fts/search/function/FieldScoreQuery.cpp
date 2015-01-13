/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldScoreQuery.h"
#include "ByteFieldSource.h"
#include "IntFieldSource.h"
#include "DoubleFieldSource.h"

namespace Lucene {

FieldScoreQuery::FieldScoreQuery(const String& field, Type type) : ValueSourceQuery(getValueSource(field,type)) {
}

FieldScoreQuery::~FieldScoreQuery() {
}

ValueSourcePtr FieldScoreQuery::getValueSource(const String& field, Type type) {
    switch (type) {
    case BYTE:
        return newLucene<ByteFieldSource>(field);
    case INT:
        return newLucene<IntFieldSource>(field);
    case DOUBLE:
        return newLucene<DoubleFieldSource>(field);
    default:
        boost::throw_exception(IllegalArgumentException(L"not a known Field Score Query Type"));
        return ValueSourcePtr();
    }
}

}
