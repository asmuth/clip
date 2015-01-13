/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSTERMSCONSUMER_H
#define FORMATPOSTINGSTERMSCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class FormatPostingsTermsConsumer : public LuceneObject {
public:
    virtual ~FormatPostingsTermsConsumer();

    LUCENE_CLASS(FormatPostingsTermsConsumer);

public:
    CharArray termBuffer;

public:
    /// Adds a new term in this field
    virtual FormatPostingsDocsConsumerPtr addTerm(CharArray text, int32_t start) = 0;
    virtual FormatPostingsDocsConsumerPtr addTerm(const String& text);

    /// Called when we are done adding terms to this field
    virtual void finish() = 0;
};

}

#endif
