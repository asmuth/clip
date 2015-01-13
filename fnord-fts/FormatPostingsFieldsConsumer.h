/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSFIELDSCONSUMER_H
#define FORMATPOSTINGSFIELDSCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

/// Abstract API that consumes terms, doc, freq, prox and payloads postings.  Concrete implementations of this
/// actually do "something" with the postings (write it into the index in a specific format).
class FormatPostingsFieldsConsumer : public LuceneObject {
public:
    virtual ~FormatPostingsFieldsConsumer();

    LUCENE_CLASS(FormatPostingsFieldsConsumer);

public:
    /// Add a new field.
    virtual FormatPostingsTermsConsumerPtr addField(const FieldInfoPtr& field) = 0;

    /// Called when we are done adding everything.
    virtual void finish() = 0;
};

}

#endif
