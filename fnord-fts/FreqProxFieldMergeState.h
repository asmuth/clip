/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FREQPROXFIELDMERGESTATE_H
#define FREQPROXFIELDMERGESTATE_H

#include "LuceneObject.h"

namespace Lucene {

/// Used by DocumentsWriter to merge the postings from multiple ThreadStates when creating a segment
class FreqProxFieldMergeState : public LuceneObject {
public:
    FreqProxFieldMergeState(const FreqProxTermsWriterPerFieldPtr& field);
    virtual ~FreqProxFieldMergeState();

    LUCENE_CLASS(FreqProxFieldMergeState);

public:
    FreqProxTermsWriterPerFieldPtr field;
    int32_t numPostings;
    CharBlockPoolPtr charPool;
    Collection<RawPostingListPtr> postings;

    FreqProxTermsWriterPostingListPtr p;
    CharArray text;
    int32_t textOffset;

    ByteSliceReaderPtr freq;
    ByteSliceReaderPtr prox;

    int32_t docID;
    int32_t termFreq;

protected:
    int32_t postingUpto;

public:
    bool nextTerm();
    bool nextDoc();
};

}

#endif
