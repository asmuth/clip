/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTTERMPOSITIONVECTOR_H
#define SEGMENTTERMPOSITIONVECTOR_H

#include "SegmentTermVector.h"

namespace Lucene {

class SegmentTermPositionVector : public SegmentTermVector {
public:
    SegmentTermPositionVector(const String& field, Collection<String> terms, Collection<int32_t> termFreqs,
                              Collection< Collection<int32_t> > positions, Collection< Collection<TermVectorOffsetInfoPtr> > offsets);
    virtual ~SegmentTermPositionVector();

    LUCENE_CLASS(SegmentTermPositionVector);

protected:
    Collection< Collection<int32_t> > positions;
    Collection< Collection<TermVectorOffsetInfoPtr> > offsets;

protected:
    static const Collection<int32_t> EMPTY_TERM_POS();

public:
    /// Returns an array of TermVectorOffsetInfo in which the term is found.
    /// @param index The position in the array to get the offsets from
    /// @return An array of TermVectorOffsetInfo objects or the empty list
    virtual Collection<TermVectorOffsetInfoPtr> getOffsets(int32_t index);

    /// Returns an array of positions in which the term is found.
    /// Terms are identified by the index at which its number appears in the term String array obtained from the indexOf method.
    virtual Collection<int32_t> getTermPositions(int32_t index);
};

}

#endif
