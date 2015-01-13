/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _INTFIELDSOURCE_H
#define _INTFIELDSOURCE_H

#include "DocValues.h"

namespace Lucene {

class IntDocValues : public DocValues {
public:
    IntDocValues(const IntFieldSourcePtr& source, Collection<int32_t> arr);
    virtual ~IntDocValues();

    LUCENE_CLASS(IntDocValues);

protected:
    IntFieldSourceWeakPtr _source;
    Collection<int32_t> arr;

public:
    virtual double doubleVal(int32_t doc);
    virtual int32_t intVal(int32_t doc);
    virtual String toString(int32_t doc);
    virtual CollectionValue getInnerArray();
};

}

#endif
