/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDCONSUMERPERTHREAD_H
#define DOCFIELDCONSUMERPERTHREAD_H

#include "LuceneObject.h"

namespace Lucene {

class DocFieldConsumerPerThread : public LuceneObject {
public:
    virtual ~DocFieldConsumerPerThread();

    LUCENE_CLASS(DocFieldConsumerPerThread);

public:
    virtual void startDocument() = 0;
    virtual DocWriterPtr finishDocument() = 0;
    virtual DocFieldConsumerPerFieldPtr addField(const FieldInfoPtr& fi) = 0;
    virtual void abort() = 0;
};

}

#endif
