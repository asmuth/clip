/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSHASHCONSUMERPERTHREAD_H
#define TERMSHASHCONSUMERPERTHREAD_H

#include "LuceneObject.h"

namespace Lucene {

class TermsHashConsumerPerThread : public LuceneObject {
public:
    virtual ~TermsHashConsumerPerThread();

    LUCENE_CLASS(TermsHashConsumerPerThread);

public:
    virtual void startDocument() = 0;
    virtual DocWriterPtr finishDocument() = 0;
    virtual TermsHashConsumerPerFieldPtr addField(const TermsHashPerFieldPtr& termsHashPerField, const FieldInfoPtr& fieldInfo) = 0;
    virtual void abort() = 0;
};

}

#endif
