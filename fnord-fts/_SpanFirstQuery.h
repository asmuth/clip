/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPANFIRSTQUERY_H
#define _SPANFIRSTQUERY_H

#include "Spans.h"

namespace Lucene {

class FirstSpans : public Spans {
public:
    FirstSpans(const SpanFirstQueryPtr& query, const SpansPtr& spans);
    virtual ~FirstSpans();

    LUCENE_CLASS(FirstSpans);

protected:
    SpanFirstQueryPtr query;
    SpansPtr spans;

public:
    virtual bool next();
    virtual bool skipTo(int32_t target);
    virtual int32_t doc();
    virtual int32_t start();
    virtual int32_t end();
    virtual Collection<ByteArray> getPayload();
    virtual bool isPayloadAvailable();
};

}

#endif
