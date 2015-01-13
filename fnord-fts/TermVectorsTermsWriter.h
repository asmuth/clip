/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMVECTORSTERMSWRITER_H
#define TERMVECTORSTERMSWRITER_H

#include "TermsHashConsumer.h"
#include "DocumentsWriter.h"
#include "RawPostingList.h"

namespace Lucene {

class TermVectorsTermsWriter : public TermsHashConsumer {
public:
    TermVectorsTermsWriter(const DocumentsWriterPtr& docWriter);
    virtual ~TermVectorsTermsWriter();

    LUCENE_CLASS(TermVectorsTermsWriter);

public:
    DocumentsWriterWeakPtr _docWriter;
    TermVectorsWriterPtr termVectorsWriter;
    Collection<TermVectorsTermsWriterPerDocPtr> docFreeList;
    int32_t freeCount;
    IndexOutputPtr tvx;
    IndexOutputPtr tvd;
    IndexOutputPtr tvf;
    int32_t lastDocID;
    int32_t allocCount;

public:
    virtual TermsHashConsumerPerThreadPtr addThread(const TermsHashPerThreadPtr& perThread);
    virtual void createPostings(Collection<RawPostingListPtr> postings, int32_t start, int32_t count);
    virtual void flush(MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);
    virtual void closeDocStore(const SegmentWriteStatePtr& state);

    TermVectorsTermsWriterPerDocPtr getPerDoc();

    /// Fills in no-term-vectors for all docs we haven't seen since the last doc that had term vectors.
    void fill(int32_t docID);

    void initTermVectorsWriter();
    void finishDocument(const TermVectorsTermsWriterPerDocPtr& perDoc);
    bool freeRAM();
    void free(const TermVectorsTermsWriterPerDocPtr& doc);

    virtual void abort();
    virtual int32_t bytesPerPosting();
};

class TermVectorsTermsWriterPerDoc : public DocWriter {
public:
    TermVectorsTermsWriterPerDoc(const TermVectorsTermsWriterPtr& termsWriter = TermVectorsTermsWriterPtr());
    virtual ~TermVectorsTermsWriterPerDoc();

    LUCENE_CLASS(TermVectorsTermsWriterPerDoc);

protected:
    TermVectorsTermsWriterWeakPtr _termsWriter;

public:
    PerDocBufferPtr buffer;
    RAMOutputStreamPtr perDocTvf;
    int32_t numVectorFields;

    Collection<int32_t> fieldNumbers;
    Collection<int64_t> fieldPointers;

public:
    void reset();
    virtual void abort();
    void addField(int32_t fieldNumber);
    virtual int64_t sizeInBytes();
    virtual void finish();
};

class TermVectorsTermsWriterPostingList : public RawPostingList {
public:
    TermVectorsTermsWriterPostingList();
    virtual ~TermVectorsTermsWriterPostingList();

    LUCENE_CLASS(TermVectorsTermsWriterPostingList);

public:
    int32_t freq; // How many times this term occurred in the current doc
    int32_t lastOffset; // Last offset we saw
    int32_t lastPosition; // Last position where this term occurred
};

}

#endif
