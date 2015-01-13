/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSDOCSCONSUMER_H
#define FORMATPOSTINGSDOCSCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class FormatPostingsDocsConsumer : public LuceneObject {
public:
    virtual ~FormatPostingsDocsConsumer();

    LUCENE_CLASS(FormatPostingsDocsConsumer);

public:
    /// Adds a new doc in this term.  If this returns null then we just skip consuming positions/payloads.
    virtual FormatPostingsPositionsConsumerPtr addDoc(int32_t docID, int32_t termDocFreq) = 0;

    /// Called when we are done adding docs to this term
    virtual void finish() = 0;
};

}

#endif
