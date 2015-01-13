/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELDCACHETERMSFILTER_H
#define _FIELDCACHETERMSFILTER_H

#include "DocIdSet.h"
#include "DocIdSetIterator.h"

namespace Lucene {

class FieldCacheTermsFilterDocIdSet : public DocIdSet {
public:
    FieldCacheTermsFilterDocIdSet(Collection<String> terms, const StringIndexPtr& fcsi);
    virtual ~FieldCacheTermsFilterDocIdSet();

    LUCENE_CLASS(FieldCacheTermsFilterDocIdSet);

protected:
    StringIndexPtr fcsi;
    OpenBitSetPtr openBitSet;

public:
    virtual DocIdSetIteratorPtr iterator();

    /// This DocIdSet implementation is cacheable.
    virtual bool isCacheable();
};

class FieldCacheTermsFilterDocIdSetIterator : public DocIdSetIterator {
public:
    FieldCacheTermsFilterDocIdSetIterator(const StringIndexPtr& fcsi, const OpenBitSetPtr& openBitSet);
    virtual ~FieldCacheTermsFilterDocIdSetIterator();

    LUCENE_CLASS(FieldCacheTermsFilterDocIdSetIterator);

protected:
    StringIndexPtr fcsi;
    OpenBitSetPtr openBitSet;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
};

}

#endif
