/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTTERMDOCS_H
#define SEGMENTTERMDOCS_H

#include "TermPositions.h"

namespace Lucene {

class SegmentTermDocs : public TermPositions, public LuceneObject {
public:
    SegmentTermDocs(const SegmentReaderPtr& parent);
    virtual ~SegmentTermDocs();

    LUCENE_CLASS(SegmentTermDocs);

protected:
    SegmentReaderWeakPtr _parent;
    IndexInputPtr _freqStream;
    int32_t count;
    int32_t df;
    BitVectorPtr deletedDocs;
    int32_t _doc;
    int32_t _freq;

    int32_t skipInterval;
    int32_t maxSkipLevels;
    DefaultSkipListReaderPtr skipListReader;

    int64_t freqBasePointer;
    int64_t proxBasePointer;

    int64_t skipPointer;
    bool haveSkipped;

    bool currentFieldStoresPayloads;
    bool currentFieldOmitTermFreqAndPositions;

public:
    /// Sets this to the data for a term.
    virtual void seek(const TermPtr& term);

    /// Sets this to the data for the current term in a {@link TermEnum}.
    virtual void seek(const TermEnumPtr& termEnum);

    virtual void seek(const TermInfoPtr& ti, const TermPtr& term);

    virtual void close();

    /// Returns the current document number.
    virtual int32_t doc();

    /// Returns the frequency of the term within the current document.
    virtual int32_t freq();

    /// Moves to the next pair in the enumeration.
    virtual bool next();

    /// Optimized implementation.
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);

    /// Optimized implementation.
    virtual bool skipTo(int32_t target);

    /// Used for testing
    virtual IndexInputPtr freqStream();
    virtual void freqStream(const IndexInputPtr& freqStream);

protected:
    virtual void skippingDoc();
    virtual int32_t readNoTf(Collection<int32_t> docs, Collection<int32_t> freqs, int32_t length);

    /// Overridden by SegmentTermPositions to skip in prox stream.
    virtual void skipProx(int64_t proxPointer, int32_t payloadLength);
};

}

#endif
