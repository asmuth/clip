/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FREQPROXTERMSWRITER_H
#define FREQPROXTERMSWRITER_H

#include "TermsHashConsumer.h"
#include "RawPostingList.h"

namespace Lucene {

class FreqProxTermsWriter : public TermsHashConsumer {
public:
    virtual ~FreqProxTermsWriter();

    LUCENE_CLASS(FreqProxTermsWriter);

protected:
    ByteArray payloadBuffer;

public:
    virtual TermsHashConsumerPerThreadPtr addThread(const TermsHashPerThreadPtr& perThread);
    virtual void createPostings(Collection<RawPostingListPtr> postings, int32_t start, int32_t count);
    virtual void closeDocStore(const SegmentWriteStatePtr& state);
    virtual void abort();
    virtual void flush(MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);

    /// Walk through all unique text tokens (Posting instances) found in this field and serialize them
    /// into a single RAM segment.
    void appendPostings(Collection<FreqProxTermsWriterPerFieldPtr> fields, const FormatPostingsFieldsConsumerPtr& consumer);

    virtual int32_t bytesPerPosting();

protected:
    static int32_t compareText(const wchar_t* text1, int32_t pos1, const wchar_t* text2, int32_t pos2);
};

class FreqProxTermsWriterPostingList : public RawPostingList {
public:
    FreqProxTermsWriterPostingList();
    virtual ~FreqProxTermsWriterPostingList();

    LUCENE_CLASS(FreqProxTermsWriterPostingList);

public:
    int32_t docFreq; // # times this term occurs in the current doc
    int32_t lastDocID; // Last docID where this term occurred
    int32_t lastDocCode; // Code for prior doc
    int32_t lastPosition; // Last position where this term occurred
};

}

#endif
