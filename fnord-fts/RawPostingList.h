/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RAWPOSTINGLIST_H
#define RAWPOSTINGLIST_H

#include "LuceneObject.h"

namespace Lucene {

/// This is the base class for an in-memory posting list, keyed by a Token.  {@link TermsHash} maintains a
/// hash table holding one instance of this per unique Token.  Consumers of TermsHash ({@link TermsHashConsumer})
/// must subclass this class with its own concrete class.  FreqProxTermsWriterPostingList is a private inner
/// class used for the freq/prox postings, and TermVectorsTermsWriterPostingList is a private inner class used
/// to hold TermVectors postings.
class RawPostingList : public LuceneObject {
public:
    RawPostingList();
    virtual ~RawPostingList();

    LUCENE_CLASS(RawPostingList);

public:
    static const int32_t BYTES_SIZE;

    int32_t textStart;
    int32_t intStart;
    int32_t byteStart;
};

}

#endif
