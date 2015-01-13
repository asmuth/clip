/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTTERMVECTOR_H
#define SEGMENTTERMVECTOR_H

#include "TermPositionVector.h"

namespace Lucene {

class SegmentTermVector : public TermPositionVector, public LuceneObject {
public:
    SegmentTermVector(const String& field, Collection<String> terms, Collection<int32_t> termFreqs);
    virtual ~SegmentTermVector();

    LUCENE_CLASS(SegmentTermVector);

protected:
    String field;
    Collection<String> terms;
    Collection<int32_t> termFreqs;

public:
    /// @return The number of the field this vector is associated with
    virtual String getField();

    virtual String toString();

    /// @return The number of terms in the term vector.
    virtual int32_t size();

    /// @return An Array of term texts in ascending order.
    virtual Collection<String> getTerms();

    /// @return Array of term frequencies.
    virtual Collection<int32_t> getTermFrequencies();

    /// Return an index in the term numbers array returned from getTerms at which the term with the
    /// specified term appears.
    virtual int32_t indexOf(const String& term);

    /// Just like indexOf(int) but searches for a number of terms at the same time.
    virtual Collection<int32_t> indexesOf(Collection<String> termNumbers, int32_t start, int32_t length);
};

}

#endif
