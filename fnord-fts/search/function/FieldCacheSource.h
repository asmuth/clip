/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHESOURCE_H
#define FIELDCACHESOURCE_H

#include "ValueSource.h"

namespace Lucene {

/// A base class for ValueSource implementations that retrieve values for a single field from the
/// {@link FieldCache}.
///
/// Fields used herein must be indexed (doesn't matter if these fields are stored or not).
///
/// It is assumed that each such indexed field is untokenized, or at least has a single token in a document.
/// For documents with multiple tokens of the same field, behavior is undefined (It is likely that current
/// code would use the value of one of these tokens, but this is not guaranteed).
///
/// Document with no tokens in this field are assigned the Zero value.
///
/// NOTE: with the switch in 2.9 to segment-based searching, if {@link #getValues} is invoked with a composite
/// (multi-segment) reader, this can easily cause double RAM usage for the values in the FieldCache.  It's
/// best to switch your application to pass only atomic (single segment) readers to this API.
class FieldCacheSource : public ValueSource {
public:
    /// Create a cached field source for the input field.
    FieldCacheSource(const String& field);
    virtual ~FieldCacheSource();

    LUCENE_CLASS(FieldCacheSource);

protected:
    String field;

public:
    virtual DocValuesPtr getValues(const IndexReaderPtr& reader);
    virtual String description();

    /// Return cached DocValues for input field and reader.
    /// @param cache FieldCache so that values of a field are loaded once per reader (RAM allowing)
    /// @param field Field for which values are required.
    /// @see ValueSource
    virtual DocValuesPtr getCachedFieldValues(const FieldCachePtr& cache, const String& field, const IndexReaderPtr& reader) = 0;

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    /// Check if equals to another {@link FieldCacheSource}, already knowing that cache and field are equal.
    virtual bool cachedFieldSourceEquals(const FieldCacheSourcePtr& other) = 0;

    /// Return a hash code of a {@link FieldCacheSource}, without the hash-codes of the field and the cache
    /// (those are taken care of elsewhere).
    virtual int32_t cachedFieldSourceHashCode() = 0;
};

}

#endif
