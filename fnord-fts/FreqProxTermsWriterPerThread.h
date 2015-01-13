/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FREQPROXTERMSWRITERPERTHREAD_H
#define FREQPROXTERMSWRITERPERTHREAD_H

#include "TermsHashConsumerPerThread.h"

namespace Lucene {

class FreqProxTermsWriterPerThread : public TermsHashConsumerPerThread {
public:
    FreqProxTermsWriterPerThread(const TermsHashPerThreadPtr& perThread);
    virtual ~FreqProxTermsWriterPerThread();

    LUCENE_CLASS(FreqProxTermsWriterPerThread);

public:
    TermsHashPerThreadWeakPtr _termsHashPerThread;
    DocStatePtr docState;

public:
    virtual TermsHashConsumerPerFieldPtr addField(const TermsHashPerFieldPtr& termsHashPerField, const FieldInfoPtr& fieldInfo);
    virtual void startDocument();
    virtual DocWriterPtr finishDocument();
    virtual void abort();
};

}

#endif
