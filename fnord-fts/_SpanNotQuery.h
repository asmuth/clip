/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPANNOTQUERY_H
#define _SPANNOTQUERY_H

#include "Spans.h"

namespace Lucene {

class NotSpans : public Spans {
public:
    NotSpans(const SpanNotQueryPtr& query, const SpansPtr& includeSpans, const SpansPtr& excludeSpans);
    virtual ~NotSpans();

    LUCENE_CLASS(NotSpans);

protected:
    SpanNotQueryPtr query;
    SpansPtr includeSpans;
    bool moreInclude;
    SpansPtr excludeSpans;
    bool moreExclude;

public:
    virtual bool next();
    virtual bool skipTo(int32_t target);
    virtual int32_t doc();
    virtual int32_t start();
    virtual int32_t end();
    virtual Collection<ByteArray> getPayload();
    virtual bool isPayloadAvailable();
    virtual String toString();
};

}

#endif
