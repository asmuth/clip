/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSPANS_H
#define TERMSPANS_H

#include "fnord-fts/search/spans/Spans.h"

namespace Lucene {

/// Public for extension only
class TermSpans : public Spans {
public:
    TermSpans(const TermPositionsPtr& positions, const TermPtr& term);
    virtual ~TermSpans();

    LUCENE_CLASS(TermSpans);

protected:
    TermPositionsPtr positions;
    TermPtr term;
    int32_t _doc;
    int32_t freq;
    int32_t count;
    int32_t position;

public:
    virtual bool next();
    virtual bool skipTo(int32_t target);
    virtual int32_t doc();
    virtual int32_t start();
    virtual int32_t end();
    virtual Collection<ByteArray> getPayload();
    virtual bool isPayloadAvailable();
    virtual String toString();

    TermPositionsPtr getPositions();
};

}

#endif
