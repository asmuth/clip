/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDCONSUMER_H
#define DOCFIELDCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class DocFieldConsumer : public LuceneObject {
public:
    virtual ~DocFieldConsumer();

    LUCENE_CLASS(DocFieldConsumer);

protected:
    FieldInfosPtr fieldInfos;

public:
    /// Called when DocumentsWriter decides to create a new segment
    virtual void flush(MapDocFieldConsumerPerThreadCollectionDocFieldConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) = 0;

    /// Called when DocumentsWriter decides to close the doc stores
    virtual void closeDocStore(const SegmentWriteStatePtr& state) = 0;

    /// Called when an aborting exception is hit
    virtual void abort() = 0;

    /// Add a new thread
    virtual DocFieldConsumerPerThreadPtr addThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread) = 0;

    /// Called when DocumentsWriter is using too much RAM.  The consumer should free RAM, if possible, returning
    /// true if any RAM was in fact freed.
    virtual bool freeRAM() = 0;

    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos);
};

}

#endif
