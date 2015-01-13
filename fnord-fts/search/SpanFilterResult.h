/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANFILTERRESULT_H
#define SPANFILTERRESULT_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// The results of a SpanQueryFilter.  Wraps the BitSet and the position information from the SpanQuery
class SpanFilterResult : public LuceneObject {
public:
    /// @param docIdSet The DocIdSet for the Filter
    /// @param positions A List of {@link PositionInfo} objects
    SpanFilterResult(const DocIdSetPtr& docIdSet, Collection<PositionInfoPtr> positions);

    virtual ~SpanFilterResult();

    LUCENE_CLASS(SpanFilterResult);

protected:
    DocIdSetPtr docIdSet;
    Collection<PositionInfoPtr> positions; // Spans spans

public:
    /// The first entry in the array corresponds to the first "on" bit.  Entries are increasing by
    /// document order.
    /// @return A List of PositionInfo objects
    Collection<PositionInfoPtr> getPositions();

    /// Returns the docIdSet
    DocIdSetPtr getDocIdSet();
};

class PositionInfo : public LuceneObject {
public:
    PositionInfo(int32_t doc);
    virtual ~PositionInfo();

    LUCENE_CLASS(PositionInfo);

protected:
    int32_t doc;
    Collection<StartEndPtr> positions;

public:
    void addPosition(int32_t start, int32_t end);
    int32_t getDoc();
    Collection<StartEndPtr> getPositions();
};

class StartEnd : public LuceneObject {
public:
    StartEnd(int32_t start, int32_t end);
    virtual ~StartEnd();

    LUCENE_CLASS(StartEnd);

protected:
    int32_t start;
    int32_t end;

public:
    /// @return The end position of this match
    int32_t getEnd();

    /// @return The start position of this match
    int32_t getStart();
};

}

#endif
