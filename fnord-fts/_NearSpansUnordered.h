/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _NEARSPANSUNORDERED_H
#define _NEARSPANSUNORDERED_H

#include "Spans.h"
#include "PriorityQueue.h"

namespace Lucene {

/// Wraps a Spans, and can be used to form a linked list.
class SpansCell : public Spans {
public:
    SpansCell(const NearSpansUnorderedPtr& unordered, const SpansPtr& spans, int32_t index);
    virtual ~SpansCell();

    LUCENE_CLASS(SpansCell);

protected:
    NearSpansUnorderedWeakPtr _unordered;
    SpansPtr spans;
    SpansCellPtr _next;
    int32_t length;
    int32_t index;

public:
    virtual bool next();
    virtual bool skipTo(int32_t target);
    virtual int32_t doc();
    virtual int32_t start();
    virtual int32_t end();
    virtual Collection<ByteArray> getPayload();
    virtual bool isPayloadAvailable();
    virtual String toString();

protected:
    bool adjust(bool condition);

    friend class NearSpansUnordered;
};

class CellQueue : public PriorityQueue<SpansCellPtr> {
public:
    CellQueue(int32_t size);
    virtual ~CellQueue();

    LUCENE_CLASS(CellQueue);

protected:
    virtual bool lessThan(const SpansCellPtr& first, const SpansCellPtr& second);
};

}

#endif
