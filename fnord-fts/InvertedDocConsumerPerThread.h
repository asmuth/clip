/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCCONSUMERPERTHREAD_H
#define INVERTEDDOCCONSUMERPERTHREAD_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocConsumerPerThread : public LuceneObject {
public:
    virtual ~InvertedDocConsumerPerThread();

    LUCENE_CLASS(InvertedDocConsumerPerThread);

public:
    virtual void startDocument() = 0;
    virtual InvertedDocConsumerPerFieldPtr addField(const DocInverterPerFieldPtr& docInverterPerField, const FieldInfoPtr& fieldInfo) = 0;
    virtual DocWriterPtr finishDocument() = 0;
    virtual void abort() = 0;
};

}

#endif
