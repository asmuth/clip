/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MINPAYLOADFUNCTION_H
#define MINPAYLOADFUNCTION_H

#include "fnord-fts/search/payloads/PayloadFunction.h"

namespace Lucene {

/// Calculates the minimum payload seen
class MinPayloadFunction : public PayloadFunction {
public:
    virtual ~MinPayloadFunction();
    LUCENE_CLASS(MinPayloadFunction);

public:
    virtual double currentScore(int32_t docId, const String& field, int32_t start, int32_t end, int32_t numPayloadsSeen,
                                double currentScore, double currentPayloadScore);
    virtual double docScore(int32_t docId, const String& field, int32_t numPayloadsSeen, double payloadScore);
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);
};

}

#endif
