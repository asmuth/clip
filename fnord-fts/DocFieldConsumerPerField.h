/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDCONSUMERPERFIELD_H
#define DOCFIELDCONSUMERPERFIELD_H

#include "LuceneObject.h"

namespace Lucene {

class DocFieldConsumerPerField : public LuceneObject {
public:
    virtual ~DocFieldConsumerPerField();

    LUCENE_CLASS(DocFieldConsumerPerField);

public:
    /// Processes all occurrences of a single field
    virtual void processFields(Collection<FieldablePtr> fields, int32_t count) = 0;

    virtual void abort() = 0;
};

}

#endif
