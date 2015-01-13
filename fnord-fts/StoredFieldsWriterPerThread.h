/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STOREDFIELDSWRITERPERTHREAD_H
#define STOREDFIELDSWRITERPERTHREAD_H

#include "LuceneObject.h"

namespace Lucene {

class StoredFieldsWriterPerThread : public LuceneObject {
public:
    StoredFieldsWriterPerThread(const DocStatePtr& docState, const StoredFieldsWriterPtr& storedFieldsWriter);
    virtual ~StoredFieldsWriterPerThread();

    LUCENE_CLASS(StoredFieldsWriterPerThread);

public:
    FieldsWriterPtr localFieldsWriter;
    StoredFieldsWriterWeakPtr _storedFieldsWriter;
    DocStatePtr docState;

    StoredFieldsWriterPerDocPtr doc;

public:
    void startDocument();
    void addField(const FieldablePtr& field, const FieldInfoPtr& fieldInfo);
    DocWriterPtr finishDocument();
    void abort();
};

}

#endif
