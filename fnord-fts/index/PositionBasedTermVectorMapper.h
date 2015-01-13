/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef POSITIONBASEDTERMVECTORMAPPER_H
#define POSITIONBASEDTERMVECTORMAPPER_H

#include "fnord-fts/index/TermVectorMapper.h"

namespace Lucene {

class PositionBasedTermVectorMapper : public TermVectorMapper {
public:
    PositionBasedTermVectorMapper(bool ignoringOffsets = false);
    virtual ~PositionBasedTermVectorMapper();

    LUCENE_CLASS(PositionBasedTermVectorMapper);

protected:
    MapStringMapIntTermVectorsPositionInfo fieldToTerms;

    String currentField;

    /// A Map of Integer and TermVectorsPositionInfo
    MapIntTermVectorsPositionInfo currentPositions;

    bool storeOffsets;

public:
    /// Never ignores positions.  This mapper doesn't make much sense unless there are positions.
    /// @return false
    virtual bool isIgnoringPositions();

    /// Callback for the TermVectorReader.
    virtual void map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions);

    /// Callback mechanism used by the TermVectorReader.
    virtual void setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions);

    /// Get the mapping between fields and terms, sorted by the comparator
    /// @return A map between field names and a Map.  The sub-Map key is the position as the integer, the value is
    /// {@link PositionBasedTermVectorMapper}.
    MapStringMapIntTermVectorsPositionInfo getFieldToTerms();
};

/// Container for a term at a position
class TermVectorsPositionInfo : public LuceneObject {
public:
    TermVectorsPositionInfo(int32_t position, bool storeOffsets);
    virtual ~TermVectorsPositionInfo();

    LUCENE_CLASS(TermVectorsPositionInfo);

protected:
    int32_t position;
    Collection<String> terms;
    Collection<TermVectorOffsetInfoPtr> offsets;

public:
    void addTerm(const String& term, const TermVectorOffsetInfoPtr& info);

    /// @return The position of the term
    int32_t getPosition();

    /// Note, there may be multiple terms at the same position
    /// @return A List of Strings
    Collection<String> getTerms();

    /// Parallel list (to {@link #getTerms()}) of TermVectorOffsetInfo objects.  There may be multiple
    /// entries since there may be multiple terms at a position.
    /// @return A List of TermVectorOffsetInfo objects, if offsets are stored.
    Collection<TermVectorOffsetInfoPtr> getOffsets();
};

}

#endif
