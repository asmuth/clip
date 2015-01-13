/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPANORQUERY_H
#define _SPANORQUERY_H

#include "PriorityQueue.h"
#include "Spans.h"

namespace Lucene {

class SpanQueue : public PriorityQueue<SpansPtr> {
public:
    SpanQueue(int32_t size);
    virtual ~SpanQueue();

    LUCENE_CLASS(SpanQueue);

protected:
    virtual bool lessThan(const SpansPtr& first, const SpansPtr& second);
};

class OrSpans : public Spans {
public:
    OrSpans(const SpanOrQueryPtr& query, const IndexReaderPtr& reader);
    virtual ~OrSpans();

    LUCENE_CLASS(OrSpans);

protected:
    SpanOrQueryPtr query;
    IndexReaderPtr reader;
    SpanQueuePtr queue;

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
    bool initSpanQueue(int32_t target);
    SpansPtr top();
};

}

#endif
