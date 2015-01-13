/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _ORDFIELDSOURCE_H
#define _ORDFIELDSOURCE_H

#include "DocValues.h"

namespace Lucene {

class LPPAPI OrdDocValues : public DocValues {
public:
    OrdDocValues(const OrdFieldSourcePtr& source, Collection<int32_t> arr);
    virtual ~OrdDocValues();

    LUCENE_CLASS(OrdDocValues);

protected:
    OrdFieldSourceWeakPtr _source;
    Collection<int32_t> arr;

public:
    virtual double doubleVal(int32_t doc);
    virtual String strVal(int32_t doc);
    virtual String toString(int32_t doc);
    virtual CollectionValue getInnerArray();
};

}

#endif
