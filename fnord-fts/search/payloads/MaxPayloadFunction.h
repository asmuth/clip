/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MAXPAYLOADFUNCTION_H
#define MAXPAYLOADFUNCTION_H

#include "PayloadFunction.h"

namespace Lucene {

/// Returns the maximum payload score seen, else 1 if there are no payloads on the doc.
///
/// Is thread safe and completely reusable.
class MaxPayloadFunction : public PayloadFunction {
public:
    virtual ~MaxPayloadFunction();
    LUCENE_CLASS(MaxPayloadFunction);

public:
    virtual double currentScore(int32_t docId, const String& field, int32_t start, int32_t end, int32_t numPayloadsSeen,
                                double currentScore, double currentPayloadScore);
    virtual double docScore(int32_t docId, const String& field, int32_t numPayloadsSeen, double payloadScore);
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);
};

}

#endif
