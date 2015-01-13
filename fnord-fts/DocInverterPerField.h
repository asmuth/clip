/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCINVERTERPERFIELD_H
#define DOCINVERTERPERFIELD_H

#include "DocFieldConsumerPerField.h"

namespace Lucene {

/// Holds state for inverting all occurrences of a single field in the document.  This class doesn't do
/// anything itself; instead, it forwards the tokens produced by analysis to its own consumer
/// (InvertedDocConsumerPerField).  It also interacts with an endConsumer (InvertedDocEndConsumerPerField).
class DocInverterPerField : public DocFieldConsumerPerField {
public:
    DocInverterPerField(const DocInverterPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo);
    virtual ~DocInverterPerField();

    LUCENE_CLASS(DocInverterPerField);

protected:
    DocInverterPerThreadWeakPtr _perThread;
    FieldInfoPtr fieldInfo;

public:
    InvertedDocConsumerPerFieldPtr consumer;
    InvertedDocEndConsumerPerFieldPtr endConsumer;
    DocStatePtr docState;
    FieldInvertStatePtr fieldState;

public:
    virtual void initialize();
    virtual void abort();

    /// Processes all occurrences of a single field
    virtual void processFields(Collection<FieldablePtr> fields, int32_t count);
};

}

#endif
