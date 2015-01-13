/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOUBLEFIELDSOURCE_H
#define DOUBLEFIELDSOURCE_H

#include "FieldCacheSource.h"
#include "DocValues.h"

namespace Lucene {

/// Obtains double field values from the {@link FieldCache} using getDoubles() and makes those values available
/// as other numeric types, casting as needed.
///
/// @see FieldCacheSource for requirements on the field.
///
/// NOTE: with the switch in 2.9 to segment-based searching, if {@link #getValues} is invoked with a composite
/// (multi-segment) reader, this can easily cause double RAM usage for the values in the FieldCache.  It's
/// best to switch your application to pass only atomic (single segment) readers to this API.  Alternatively,
/// for a short-term fix, you could wrap your ValueSource using {@link MultiValueSource}, which costs more CPU
/// per lookup but will not consume double the FieldCache RAM.
class DoubleFieldSource : public FieldCacheSource {
public:
    /// Create a cached double field source with a specific string-to-double parser.
    DoubleFieldSource(const String& field, const DoubleParserPtr& parser = DoubleParserPtr());
    virtual ~DoubleFieldSource();

    LUCENE_CLASS(DoubleFieldSource);

protected:
    DoubleParserPtr parser;

public:
    virtual String description();
    virtual DocValuesPtr getCachedFieldValues(const FieldCachePtr& cache, const String& field, const IndexReaderPtr& reader);
    virtual bool cachedFieldSourceEquals(const FieldCacheSourcePtr& other);
    virtual int32_t cachedFieldSourceHashCode();
};

class DoubleDocValues : public DocValues {
public:
    DoubleDocValues(const DoubleFieldSourcePtr& source, Collection<double> arr);
    virtual ~DoubleDocValues();

    LUCENE_CLASS(DoubleDocValues);

protected:
    DoubleFieldSourceWeakPtr _source;
    Collection<double> arr;

public:
    virtual double doubleVal(int32_t doc);
    virtual String toString(int32_t doc);
    virtual CollectionValue getInnerArray();
};

}

#endif
