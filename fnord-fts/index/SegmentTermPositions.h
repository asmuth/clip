/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTTERMPOSITIONS_H
#define SEGMENTTERMPOSITIONS_H

#include "SegmentTermDocs.h"

namespace Lucene {

class SegmentTermPositions : public SegmentTermDocs {
public:
    SegmentTermPositions(const SegmentReaderPtr& parent);
    virtual ~SegmentTermPositions();

    LUCENE_CLASS(SegmentTermPositions);

protected:
    IndexInputPtr proxStream;
    int32_t proxCount;
    int32_t position;

    /// The current payload length
    int32_t payloadLength;

    /// Indicates whether the payload of the current position has been read from the proxStream yet
    bool needToLoadPayload;

    // these variables are being used to remember information for a lazy skip
    int64_t lazySkipPointer;
    int32_t lazySkipProxCount;

public:
    using SegmentTermDocs::seek;

    virtual void seek(const TermInfoPtr& ti, const TermPtr& term);
    virtual void close();

    /// Returns next position in the current document.
    virtual int32_t nextPosition();

    /// Moves to the next pair in the enumeration.
    virtual bool next();

    /// Not supported
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);

    /// Returns the length of the payload at the current term position.
    virtual int32_t getPayloadLength();

    /// Returns the payload data at the current term position.
    virtual ByteArray getPayload(ByteArray data, int32_t offset);

    /// Checks if a payload can be loaded at this position.
    virtual bool isPayloadAvailable();

protected:
    int32_t readDeltaPosition();

    virtual void skippingDoc();

    virtual void skipProx(int64_t proxPointer, int32_t payloadLength);
    virtual void skipPositions(int32_t n);
    virtual void skipPayload();

    /// It is not always necessary to move the prox pointer to a new document after the freq pointer has
    /// been moved.  Consider for example a phrase query with two terms: the freq pointer for term 1 has to
    /// move to document x to answer the question if the term occurs in that document.  But only if term 2
    /// also matches document x, the positions have to be read to figure out if term 1 and term 2 appear next
    /// to each other in document x and thus satisfy the query.  So we move the prox pointer lazily to the
    /// document as soon as positions are requested.
    virtual void lazySkip();
};

}

#endif
