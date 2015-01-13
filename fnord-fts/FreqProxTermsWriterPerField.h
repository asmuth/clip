/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FREQPROXTERMSWRITERPERFIELD_H
#define FREQPROXTERMSWRITERPERFIELD_H

#include "TermsHashConsumerPerField.h"

namespace Lucene {

class FreqProxTermsWriterPerField : public TermsHashConsumerPerField {
public:
    FreqProxTermsWriterPerField(const TermsHashPerFieldPtr& termsHashPerField, const FreqProxTermsWriterPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo);
    virtual ~FreqProxTermsWriterPerField();

    LUCENE_CLASS(FreqProxTermsWriterPerField);

public:
    FreqProxTermsWriterPerThreadWeakPtr _perThread;
    TermsHashPerFieldWeakPtr _termsHashPerField;
    FieldInfoPtr fieldInfo;
    DocStatePtr docState;
    FieldInvertStatePtr fieldState;
    bool omitTermFreqAndPositions;
    PayloadAttributePtr payloadAttribute;
    bool hasPayloads;

public:
    virtual int32_t getStreamCount();
    virtual void finish();
    virtual void skippingLongTerm();
    virtual int32_t compareTo(const LuceneObjectPtr& other);
    void reset();
    virtual bool start(Collection<FieldablePtr> fields, int32_t count);
    virtual void start(const FieldablePtr& field);
    void writeProx(const FreqProxTermsWriterPostingListPtr& p, int32_t proxCode);
    virtual void newTerm(const RawPostingListPtr& p);
    virtual void addTerm(const RawPostingListPtr& p);
    void abort();
};

}

#endif
