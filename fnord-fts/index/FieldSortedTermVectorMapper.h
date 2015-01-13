/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDSORTEDTERMVECTORMAPPER_H
#define FIELDSORTEDTERMVECTORMAPPER_H

#include <boost/function.hpp>
#include "fnord-fts/index/TermVectorMapper.h"

namespace Lucene {

/// For each Field, store a sorted collection of {@link TermVectorEntry}s
/// This is not thread-safe.
class FieldSortedTermVectorMapper : public TermVectorMapper {
public:
    /// @param comparator A Comparator for sorting {@link TermVectorEntry}s
    FieldSortedTermVectorMapper(TermVectorEntryComparator comparator);

    FieldSortedTermVectorMapper(bool ignoringPositions, bool ignoringOffsets, TermVectorEntryComparator comparator);

    virtual ~FieldSortedTermVectorMapper();

    LUCENE_CLASS(FieldSortedTermVectorMapper);

protected:
    MapStringCollectionTermVectorEntry fieldToTerms;
    Collection<TermVectorEntryPtr> currentSet;
    String currentField;
    TermVectorEntryComparator comparator;

public:
    /// Map the Term Vector information into your own structure
    virtual void map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions);

    /// Tell the mapper what to expect in regards to field, number of terms, offset and position storage.
    virtual void setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions);

    /// Get the mapping between fields and terms, sorted by the comparator
    /// @return A map between field names and {@link java.util.SortedSet}s per field.  SortedSet entries are
    /// {@link TermVectorEntry}
    MapStringCollectionTermVectorEntry getFieldToTerms();

    TermVectorEntryComparator getComparator();
};

}

#endif
