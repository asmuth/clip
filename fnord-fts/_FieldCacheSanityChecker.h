/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELDCACHESANITYCHECKER_H
#define _FIELDCACHESANITYCHECKER_H

#include "LuceneObject.h"

namespace Lucene {

/// Simple pair object for using "readerKey + fieldName" a Map key
class ReaderField : public LuceneObject {
public:
    ReaderField(const LuceneObjectPtr& readerKey, const String& fieldName);
    virtual ~ReaderField();

    LUCENE_CLASS(ReaderField);

public:
    LuceneObjectPtr readerKey;
    String fieldName;

public:
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual String toString();
};

}

#endif
