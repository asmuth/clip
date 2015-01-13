/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCCONSUMER_H
#define INVERTEDDOCCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocConsumer : public LuceneObject {
public:
    virtual ~InvertedDocConsumer();

    LUCENE_CLASS(InvertedDocConsumer);

public:
    FieldInfosPtr fieldInfos;

public:
    /// Add a new thread
    virtual InvertedDocConsumerPerThreadPtr addThread(const DocInverterPerThreadPtr& docInverterPerThread) = 0;

    /// Abort (called after hitting AbortException)
    virtual void abort() = 0;

    /// Flush a new segment
    virtual void flush(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) = 0;

    /// Close doc stores
    virtual void closeDocStore(const SegmentWriteStatePtr& state) = 0;

    /// Attempt to free RAM, returning true if any RAM was freed
    virtual bool freeRAM() = 0;

    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos);
};

}

#endif
