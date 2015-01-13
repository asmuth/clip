/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSHASHPERTHREAD_H
#define TERMSHASHPERTHREAD_H

#include "InvertedDocConsumerPerThread.h"

namespace Lucene {

class TermsHashPerThread : public InvertedDocConsumerPerThread {
public:
    TermsHashPerThread(const DocInverterPerThreadPtr& docInverterPerThread, const TermsHashPtr& termsHash, const TermsHashPtr& nextTermsHash, const TermsHashPerThreadPtr& primaryPerThread);
    virtual ~TermsHashPerThread();

    LUCENE_CLASS(TermsHashPerThread);

public:
    DocInverterPerThreadWeakPtr _docInverterPerThread;
    TermsHashWeakPtr _termsHash;
    TermsHashPtr nextTermsHash;
    TermsHashPerThreadWeakPtr _primaryPerThread;
    TermsHashConsumerPerThreadPtr consumer;
    TermsHashPerThreadPtr nextPerThread;

    CharBlockPoolPtr charPool;
    IntBlockPoolPtr intPool;
    ByteBlockPoolPtr bytePool;
    bool primary;
    DocStatePtr docState;

    Collection<RawPostingListPtr> freePostings;
    int32_t freePostingsCount;

public:
    virtual void initialize();

    virtual InvertedDocConsumerPerFieldPtr addField(const DocInverterPerFieldPtr& docInverterPerField, const FieldInfoPtr& fieldInfo);
    virtual void abort();

    /// perField calls this when it needs more postings
    void morePostings();

    virtual void startDocument();
    virtual DocWriterPtr finishDocument();

    /// Clear all state
    void reset(bool recyclePostings);

protected:
    static bool noNullPostings(Collection<RawPostingListPtr> postings, int32_t count, const String& details);
};

}

#endif
