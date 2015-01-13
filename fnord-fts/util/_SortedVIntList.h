/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SORTEDVINTLIST_H
#define _SORTEDVINTLIST_H

#include "DocIdSetIterator.h"

namespace Lucene {

class SortedDocIdSetIterator : public DocIdSetIterator {
public:
    SortedDocIdSetIterator(const SortedVIntListPtr& list);
    virtual ~SortedDocIdSetIterator();

    LUCENE_CLASS(SortedDocIdSetIterator);

public:
    SortedVIntListWeakPtr _list;
    int32_t bytePos;
    int32_t lastInt;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);

protected:
    void advance();
};

}

#endif
