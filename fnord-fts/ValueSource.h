/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef VALUESOURCE_H
#define VALUESOURCE_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Source of values for basic function queries.
///
/// At its default/simplest form, values - one per doc - are used as the score of that doc.
///
/// Values are instantiated as {@link DocValues} for a particular reader.
/// ValueSource implementations differ in RAM requirements: it would always be a factor of the number of
/// documents, but for each document the number of bytes can be 1, 2, 4, or 8.
class ValueSource : public LuceneObject {
public:
    virtual ~ValueSource();
    LUCENE_CLASS(ValueSource);

public:
    /// Return the DocValues used by the function query.
    /// @param reader The IndexReader used to read these values.  If any caching is involved, that caching
    /// would also be IndexReader based.
    virtual DocValuesPtr getValues(const IndexReaderPtr& reader) = 0;

    /// Description of field, used in explain()
    virtual String description() = 0;

    virtual String toString();

    /// Needed for possible caching of query results - used by {@link ValueSourceQuery#equals(LuceneObjectPtr)}.
    virtual bool equals(const LuceneObjectPtr& other) = 0;

    /// Needed for possible caching of query results - used by {@link ValueSourceQuery#hashCode()}.
    virtual int32_t hashCode() = 0;
};

}

#endif
