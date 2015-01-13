/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMDOCS_H
#define TERMDOCS_H

#include "LuceneObject.h"

namespace Lucene {

/// TermDocs provides an interface for enumerating <document, frequency>; pairs for a term.  The document
/// portion names each document containing the term.  Documents are indicated by number.  The frequency
/// portion gives the number of times the term occurred in each document.  The pairs are ordered by document
/// number.
/// @see IndexReader#termDocs()
class TermDocs {
protected:
    TermDocs();

public:
    LUCENE_INTERFACE(TermDocs);

public:
    /// Sets this to the data for a term.  The enumeration is reset to the start of the data for this term.
    virtual void seek(const TermPtr& term) = 0;

    /// Sets this to the data for the current term in a {@link TermEnum}.
    /// This may be optimized in some implementations.
    virtual void seek(const TermEnumPtr& termEnum) = 0;

    /// Returns the current document number.  This is invalid until {@link #next()} is called for the first time.
    virtual int32_t doc() = 0;

    /// Returns the frequency of the term within the current document.  This is invalid until {@link #next()} is
    /// called for the first time.
    virtual int32_t freq() = 0;

    /// Moves to the next pair in the enumeration.  Returns true if there is such a next pair in the enumeration.
    virtual bool next() = 0;

    /// Attempts to read multiple entries from the enumeration, up to length of docs.  Document numbers are stored
    /// in docs, and term frequencies are stored in freqs.  Returns the number of entries read.  Zero is only
    /// returned when the stream has been exhausted.
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs) = 0;

    /// Skips entries to the first beyond the current whose document number is greater than or equal to target.
    /// Returns true if there is such an entry.
    virtual bool skipTo(int32_t target) = 0;

    /// Frees associated resources.
    virtual void close() = 0;
};

}

#endif
