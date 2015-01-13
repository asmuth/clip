/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _BYTEFIELDSOURCE_H
#define _BYTEFIELDSOURCE_H

#include "DocValues.h"

namespace Lucene {

class ByteDocValues : public DocValues {
public:
    ByteDocValues(const ByteFieldSourcePtr& source, Collection<uint8_t> arr);
    virtual ~ByteDocValues();

    LUCENE_CLASS(ByteDocValues);

protected:
    ByteFieldSourceWeakPtr _source;
    Collection<uint8_t> arr;

public:
    virtual double doubleVal(int32_t doc);
    virtual int32_t intVal(int32_t doc);
    virtual String toString(int32_t doc);
    virtual CollectionValue getInnerArray();
};

}

#endif
