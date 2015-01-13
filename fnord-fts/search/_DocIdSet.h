/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _DOCIDSET_H
#define _DOCIDSET_H

#include "DocIdSetIterator.h"

namespace Lucene {

class EmptyDocIdSetIterator : public DocIdSetIterator {
public:
    virtual ~EmptyDocIdSetIterator();
    LUCENE_CLASS(EmptyDocIdSetIterator);

public:
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();
    virtual int32_t nextDoc();
};

/// An empty {@code DocIdSet} instance for easy use, eg. in Filters that hit no documents.
class EmptyDocIdSet : public DocIdSet {
public:
    virtual ~EmptyDocIdSet();
    LUCENE_CLASS(EmptyDocIdSet);

public:
    virtual DocIdSetIteratorPtr iterator();
    virtual bool isCacheable();
};

}

#endif
