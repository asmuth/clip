/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PAYLOADATTRIBUTE_H
#define PAYLOADATTRIBUTE_H

#include "Attribute.h"

namespace Lucene {

/// The start and end character offset of a Token.
class PayloadAttribute : public Attribute {
public:
    /// Initialize this attribute with no payload.
    PayloadAttribute();

    /// Initialize this attribute with the given payload.
    PayloadAttribute(const PayloadPtr& payload);

    virtual ~PayloadAttribute();

    LUCENE_CLASS(PayloadAttribute);

protected:
    PayloadPtr payload;

public:
    virtual String toString();

    /// Returns this Token's payload.
    virtual PayloadPtr getPayload();

    /// Sets this Token's payload.
    virtual void setPayload(const PayloadPtr& payload);

    virtual void clear();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual void copyTo(const AttributePtr& target);
};

}

#endif
