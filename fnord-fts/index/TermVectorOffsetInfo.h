/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMVECTOROFFSETINFO_H
#define TERMVECTOROFFSETINFO_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// The TermVectorOffsetInfo class holds information pertaining to a Term in a {@link TermPositionVector}'s
/// offset information.  This offset information is the character offset as set during the Analysis phase
/// (and thus may not be the actual offset in the original content).
class TermVectorOffsetInfo : public LuceneObject {
public:
    TermVectorOffsetInfo(int32_t startOffset = 0, int32_t endOffset = 0);
    virtual ~TermVectorOffsetInfo();

    LUCENE_CLASS(TermVectorOffsetInfo);

protected:
    int32_t startOffset;
    int32_t endOffset;

public:
    /// Convenience declaration when creating a {@link TermPositionVector} that stores only position information.
    static const Collection<TermVectorOffsetInfoPtr> EMPTY_OFFSET_INFO();

    /// The accessor for the ending offset for the term
    int32_t getEndOffset();
    void setEndOffset(int32_t endOffset);

    /// The accessor for the starting offset of the term.
    int32_t getStartOffset();
    void setStartOffset(int32_t startOffset);

    /// Two TermVectorOffsetInfos are equals if both the start and end offsets are the same.
    /// @return true if both {@link #getStartOffset()} and {@link #getEndOffset()} are the same for both objects.
    virtual bool equals(const LuceneObjectPtr& other);

    virtual int32_t hashCode();
};

}

#endif
