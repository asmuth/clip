/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCCONSUMERPERFIELD_H
#define INVERTEDDOCCONSUMERPERFIELD_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocConsumerPerField : public LuceneObject {
public:
    virtual ~InvertedDocConsumerPerField();

    LUCENE_CLASS(InvertedDocConsumerPerField);

public:
    /// Called once per field, and is given all Fieldable occurrences for this field in the document.
    /// Return true if you wish to see inverted tokens for these fields
    virtual bool start(Collection<FieldablePtr> fields, int32_t count) = 0;

    /// Called before a field instance is being processed
    virtual void start(const FieldablePtr& field) = 0;

    /// Called once per inverted token
    virtual void add() = 0;

    /// Called once per field per document, after all Fieldable occurrences are inverted
    virtual void finish() = 0;

    /// Called on hitting an aborting exception
    virtual void abort() = 0;
};

}

#endif
