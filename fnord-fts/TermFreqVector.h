/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMFREQVECTOR_H
#define TERMFREQVECTOR_H

#include "LuceneObject.h"

namespace Lucene {

/// Provides access to stored term vector of a document field.  The vector consists of the name of the field, an
/// array of the terms that occur in the field of the {@link Document} and a parallel array of frequencies.  Thus,
/// getTermFrequencies()[5] corresponds with the frequency of getTerms()[5], assuming there are at least 5 terms
/// in the Document.
class TermFreqVector {
protected:
    TermFreqVector();

public:
    virtual ~TermFreqVector();
    LUCENE_INTERFACE(TermFreqVector);

public:
    /// The {@link Fieldable} name.
    /// @return The name of the field this vector is associated with.
    virtual String getField();

    /// @return The number of terms in the term vector.
    virtual int32_t size();

    /// @return An Array of term texts in ascending order.
    virtual Collection<String> getTerms();

    /// Array of term frequencies. Locations of the array correspond one to one to the terms in the array obtained from
    /// getTerms method. Each location in the array contains the number of times this term occurs in the document or the
    /// document field.
    virtual Collection<int32_t> getTermFrequencies();

    /// Return an index in the term numbers array returned from getTerms at which the term with the specified term appears.
    /// If this term does not appear in the array, return -1.
    virtual int32_t indexOf(const String& term);

    /// Just like indexOf(int) but searches for a number of terms at the same time. Returns an array that has the same size
    /// as the number of terms searched for, each slot containing the result of searching for that term number.
    ///
    /// @param terms array containing terms to look for
    /// @param start index in the array where the list of terms starts
    /// @param length the number of terms in the list
    virtual Collection<int32_t> indexesOf(Collection<String> terms, int32_t start, int32_t length);
};

}

#endif
