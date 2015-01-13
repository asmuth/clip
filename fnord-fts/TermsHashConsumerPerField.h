/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSHASHCONSUMERPERFIELD_H
#define TERMSHASHCONSUMERPERFIELD_H

#include "LuceneObject.h"

namespace Lucene {

/// Implement this class to plug into the TermsHash processor, which inverts & stores Tokens into a hash
/// table and provides an API for writing bytes into multiple streams for each unique Token.
class TermsHashConsumerPerField : public LuceneObject {
public:
    virtual ~TermsHashConsumerPerField();

    LUCENE_CLASS(TermsHashConsumerPerField);

public:
    virtual bool start(Collection<FieldablePtr> fields, int32_t count) = 0;
    virtual void finish() = 0;
    virtual void skippingLongTerm() = 0;
    virtual void start(const FieldablePtr& field) = 0;
    virtual void newTerm(const RawPostingListPtr& p) = 0;
    virtual void addTerm(const RawPostingListPtr& p) = 0;
    virtual int32_t getStreamCount() = 0;
};

}

#endif
