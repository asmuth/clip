/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDCONSUMERSPERTHREAD_H
#define DOCFIELDCONSUMERSPERTHREAD_H

#include "DocFieldConsumerPerThread.h"

namespace Lucene {

class DocFieldConsumersPerThread : public DocFieldConsumerPerThread {
public:
    DocFieldConsumersPerThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread, const DocFieldConsumersPtr& parent,
                               const DocFieldConsumerPerThreadPtr& one, const DocFieldConsumerPerThreadPtr& two);
    virtual ~DocFieldConsumersPerThread();

    LUCENE_CLASS(DocFieldConsumersPerThread);

public:
    DocFieldConsumerPerThreadPtr one;
    DocFieldConsumerPerThreadPtr two;
    DocFieldConsumersWeakPtr _parent;
    DocStatePtr docState;

public:
    virtual void startDocument();
    virtual void abort();
    virtual DocWriterPtr finishDocument();
    virtual DocFieldConsumerPerFieldPtr addField(const FieldInfoPtr& fi);
};

}

#endif
