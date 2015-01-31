/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SORTEDTERMVECTORMAPPER_H
#define SORTEDTERMVECTORMAPPER_H

#include <boost/function.hpp>
#include "fnord-fts/index/TermVectorMapper.h"

namespace fnord {
namespace fts {

/// Store a sorted collection of {@link TermVectorEntry}s.  Collects all term information into a single,
/// sorted set.
///
/// NOTE: This Mapper ignores all Field information for the Document.  This means that if you are using offset/
/// positions you will not know what Fields they correlate with.
///
/// This is not thread-safe
class SortedTermVectorMapper : public TermVectorMapper {
public:
    /// @param comparator A Comparator for sorting {@link TermVectorEntry}s
    SortedTermVectorMapper(TermVectorEntryComparator comparator);

    SortedTermVectorMapper(bool ignoringPositions, bool ignoringOffsets, TermVectorEntryComparator comparator);

    virtual ~SortedTermVectorMapper();

    LUCENE_CLASS(SortedTermVectorMapper);

protected:
    Collection<TermVectorEntryPtr> currentSet;
    MapStringTermVectorEntry termToTVE;
    bool storeOffsets;
    bool storePositions;
    TermVectorEntryComparator comparator;

public:
    static const wchar_t* ALL;

public:
    /// Map the Term Vector information into your own structure
    virtual void map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions);

    /// Tell the mapper what to expect in regards to field, number of terms, offset and position storage.
    virtual void setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions);

    /// The TermVectorEntrySet.  A SortedSet of {@link TermVectorEntry} objects.  Sort is by the comparator passed
    /// into the constructor.
    ///
    /// This set will be empty until after the mapping process takes place.
    ///
    /// @return The sorted set of {@link TermVectorEntry}.
    Collection<TermVectorEntryPtr> getTermVectorEntrySet();
};

}

}
#endif
