/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCENDCONSUMER_H
#define INVERTEDDOCENDCONSUMER_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocEndConsumer : public LuceneObject {
public:
    virtual ~InvertedDocEndConsumer();

    LUCENE_CLASS(InvertedDocEndConsumer);

public:
    virtual InvertedDocEndConsumerPerThreadPtr addThread(const DocInverterPerThreadPtr& docInverterPerThread) = 0;
    virtual void flush(MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) = 0;
    virtual void closeDocStore(const SegmentWriteStatePtr& state) = 0;
    virtual void abort() = 0;
    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos) = 0;
};

}

#endif
