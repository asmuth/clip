/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NORMSWRITERPERTHREAD_H
#define NORMSWRITERPERTHREAD_H

#include "InvertedDocEndConsumerPerThread.h"

namespace Lucene {

class NormsWriterPerThread : public InvertedDocEndConsumerPerThread {
public:
    NormsWriterPerThread(const DocInverterPerThreadPtr& docInverterPerThread, const NormsWriterPtr& normsWriter);
    virtual ~NormsWriterPerThread();

    LUCENE_CLASS(NormsWriterPerThread);

public:
    NormsWriterWeakPtr _normsWriter;
    DocStatePtr docState;

public:
    virtual InvertedDocEndConsumerPerFieldPtr addField(const DocInverterPerFieldPtr& docInverterPerField, const FieldInfoPtr& fieldInfo);
    virtual void abort();
    virtual void startDocument();
    virtual void finishDocument();

    bool freeRAM();
};

}

#endif
