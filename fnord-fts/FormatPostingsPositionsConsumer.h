/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSPOSITIONSCONSUMER_H
#define FORMATPOSTINGSPOSITIONSCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class FormatPostingsPositionsConsumer : public LuceneObject {
public:
    virtual ~FormatPostingsPositionsConsumer();

    LUCENE_CLASS(FormatPostingsPositionsConsumer);

public:
    /// Add a new position & payload.  If payloadLength > 0 you must read those bytes from the IndexInput.
    virtual void addPosition(int32_t position, ByteArray payload, int32_t payloadOffset, int32_t payloadLength) = 0;

    /// Called when we are done adding positions & payloads.
    virtual void finish() = 0;
};

}

#endif
