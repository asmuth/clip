/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCENDCONSUMERPERTHREAD_H
#define INVERTEDDOCENDCONSUMERPERTHREAD_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocEndConsumerPerThread : public LuceneObject {
public:
    virtual ~InvertedDocEndConsumerPerThread();

    LUCENE_CLASS(InvertedDocEndConsumerPerThread);

public:
    virtual void startDocument() = 0;
    virtual InvertedDocEndConsumerPerFieldPtr addField(const DocInverterPerFieldPtr& docInverterPerField, const FieldInfoPtr& fieldInfo) = 0;
    virtual void finishDocument() = 0;
    virtual void abort() = 0;
};

}

#endif
