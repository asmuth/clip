/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INVERTEDDOCENDCONSUMERPERFIELD_H
#define INVERTEDDOCENDCONSUMERPERFIELD_H

#include "LuceneObject.h"

namespace Lucene {

class InvertedDocEndConsumerPerField : public LuceneObject {
public:
    virtual ~InvertedDocEndConsumerPerField();

    LUCENE_CLASS(InvertedDocEndConsumerPerField);

public:
    virtual void finish() = 0;
    virtual void abort() = 0;
};

}

#endif
