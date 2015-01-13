/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCCONSUMER_H
#define DOCCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class DocConsumer : public LuceneObject {
public:
    virtual ~DocConsumer();

    LUCENE_CLASS(DocConsumer);

public:
    virtual DocConsumerPerThreadPtr addThread(const DocumentsWriterThreadStatePtr& perThread) = 0;
    virtual void flush(Collection<DocConsumerPerThreadPtr> threads, const SegmentWriteStatePtr& state) = 0;
    virtual void closeDocStore(const SegmentWriteStatePtr& state) = 0;
    virtual void abort() = 0;
    virtual bool freeRAM() = 0;
};

}

#endif
