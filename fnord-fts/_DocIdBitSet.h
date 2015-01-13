/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _DOCIDBITSET_H
#define _DOCIDBITSET_H

#include "DocIdSet.h"

namespace Lucene {

class DocIdBitSetIterator : public DocIdSetIterator {
public:
    DocIdBitSetIterator(const BitSetPtr& bitSet);
    virtual ~DocIdBitSetIterator();

    LUCENE_CLASS(DocIdBitSetIterator);

protected:
    int32_t docId;
    BitSetPtr bitSet;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
};

}

#endif
