/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSHASH_H
#define TERMSHASH_H

#include "InvertedDocConsumer.h"

namespace Lucene {

/// This class implements {@link InvertedDocConsumer}, which is passed each token produced by the analyzer on
/// each field.  It stores these tokens in a hash table, and allocates separate byte streams per token.  Consumers
/// of this class, eg {@link FreqProxTermsWriter} and {@link TermVectorsTermsWriter}, write their own byte streams
/// under each term.
class TermsHash : public InvertedDocConsumer {
public:
    TermsHash(const DocumentsWriterPtr& docWriter, bool trackAllocations, const TermsHashConsumerPtr& consumer, const TermsHashPtr& nextTermsHash);
    virtual ~TermsHash();

    LUCENE_CLASS(TermsHash);

public:
    TermsHashConsumerPtr consumer;
    TermsHashPtr nextTermsHash;
    int32_t bytesPerPosting;
    int32_t postingsFreeChunk;
    DocumentsWriterWeakPtr _docWriter;
    bool trackAllocations;

protected:
    Collection<RawPostingListPtr> postingsFreeList;
    int32_t postingsFreeCount;
    int32_t postingsAllocCount;

public:
    /// Add a new thread
    virtual InvertedDocConsumerPerThreadPtr addThread(const DocInverterPerThreadPtr& docInverterPerThread);
    virtual TermsHashPerThreadPtr addThread(const DocInverterPerThreadPtr& docInverterPerThread, const TermsHashPerThreadPtr& primaryPerThread);

    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos);

    /// Abort (called after hitting AbortException)
    /// NOTE: do not make this sync'd; it's not necessary (DW ensures all other threads are idle), and it
    /// leads to deadlock
    virtual void abort();

    void shrinkFreePostings(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);

    /// Close doc stores
    virtual void closeDocStore(const SegmentWriteStatePtr& state);

    /// Flush a new segment
    virtual void flush(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);

    /// Attempt to free RAM, returning true if any RAM was freed
    virtual bool freeRAM();

    void recyclePostings(Collection<RawPostingListPtr> postings, int32_t numPostings);

    void getPostings(Collection<RawPostingListPtr> postings);
};

}

#endif
