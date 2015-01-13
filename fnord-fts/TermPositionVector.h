/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMPOSITIONVECTOR_H
#define TERMPOSITIONVECTOR_H

#include "TermFreqVector.h"

namespace Lucene {

/// Extends TermFreqVector to provide additional information about positions in which each of the terms is found. A TermPositionVector not necessarily
/// contains both positions and offsets, but at least one of these arrays exists.
class TermPositionVector : public TermFreqVector {
protected:
    TermPositionVector();

public:
    virtual ~TermPositionVector();
    LUCENE_INTERFACE(TermPositionVector);

public:
    /// Returns an array of positions in which the term is found.  Terms are identified by the index at which its number appears in the term String
    /// array obtained from the indexOf method.  May return null if positions have not been stored.
    virtual Collection<int32_t> getTermPositions(int32_t index);

    /// Returns an array of TermVectorOffsetInfo in which the term is found.  May return null if offsets have not been stored.
    /// @see Token
    /// @param index The position in the array to get the offsets from
    /// @return An array of TermVectorOffsetInfo objects or the empty list
    virtual Collection<TermVectorOffsetInfoPtr> getOffsets(int32_t index);
};

}

#endif
