/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDPROCESSORPERFIELD_H
#define DOCFIELDPROCESSORPERFIELD_H

#include "LuceneObject.h"

namespace Lucene {

/// Holds all per thread, per field state.
class DocFieldProcessorPerField : public LuceneObject {
public:
    DocFieldProcessorPerField(const DocFieldProcessorPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo);
    virtual ~DocFieldProcessorPerField();

    LUCENE_CLASS(DocFieldProcessorPerField);

public:
    DocFieldConsumerPerFieldPtr consumer;
    FieldInfoPtr fieldInfo;

    DocFieldProcessorPerFieldPtr next;
    int32_t lastGen;

    int32_t fieldCount;
    Collection<FieldablePtr> fields;

public:
    virtual void abort();
};

}

#endif
