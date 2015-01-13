/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDCONSUMERS_H
#define DOCFIELDCONSUMERS_H

#include "DocFieldConsumer.h"
#include "DocumentsWriter.h"

namespace Lucene {

/// This is just a "splitter" class: it lets you wrap two DocFieldConsumer instances as a single consumer.
class DocFieldConsumers : public DocFieldConsumer {
public:
    DocFieldConsumers(const DocFieldConsumerPtr& one, const DocFieldConsumerPtr& two);
    virtual ~DocFieldConsumers();

    LUCENE_CLASS(DocFieldConsumers);

public:
    DocFieldConsumerPtr one;
    DocFieldConsumerPtr two;

    Collection<DocFieldConsumersPerDocPtr> docFreeList;
    int32_t freeCount;
    int32_t allocCount;

public:
    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos);

    /// Called when DocumentsWriter decides to create a new segment
    virtual void flush(MapDocFieldConsumerPerThreadCollectionDocFieldConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);

    /// Called when DocumentsWriter decides to close the doc stores
    virtual void closeDocStore(const SegmentWriteStatePtr& state);

    /// Called when DocumentsWriter is using too much RAM.
    virtual bool freeRAM();

    /// Add a new thread
    virtual DocFieldConsumerPerThreadPtr addThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread);

    DocFieldConsumersPerDocPtr getPerDoc();
    void freePerDoc(const DocFieldConsumersPerDocPtr& perDoc);
};

class DocFieldConsumersPerDoc : public DocWriter {
public:
    DocFieldConsumersPerDoc(const DocFieldConsumersPtr& fieldConsumers);
    virtual ~DocFieldConsumersPerDoc();

    LUCENE_CLASS(DocFieldConsumersPerDoc);

protected:
    DocFieldConsumersWeakPtr _fieldConsumers;

public:
    DocWriterPtr one;
    DocWriterPtr two;

public:
    virtual int64_t sizeInBytes();
    virtual void finish();
    virtual void abort();
};

}

#endif
