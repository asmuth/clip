/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef OFFSETATTRIBUTE_H
#define OFFSETATTRIBUTE_H

#include "Attribute.h"

namespace Lucene {

/// The start and end character offset of a Token.
class OffsetAttribute : public Attribute {
public:
    OffsetAttribute();
    virtual ~OffsetAttribute();

    LUCENE_CLASS(OffsetAttribute);

protected:
    int32_t _startOffset;
    int32_t _endOffset;

public:
    virtual String toString();

    /// Returns this Token's starting offset, the position of the first character corresponding to this token
    /// in the source text.
    ///
    /// Note that the difference between endOffset() and startOffset() may not be equal to termText.length(),
    /// as the term text may have been altered by a stemmer or some other filter.
    virtual int32_t startOffset();

    /// Set the starting and ending offset.
    /// @see #startOffset() and #endOffset()
    virtual void setOffset(int32_t startOffset, int32_t endOffset);

    /// Returns this Token's ending offset, one greater than the position of the last character corresponding
    /// to this token in the source text. The length of the token in the source text is (endOffset - startOffset).
    virtual int32_t endOffset();

    virtual void clear();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual void copyTo(const AttributePtr& target);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
