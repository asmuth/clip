/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _QUERYWRAPPERFILTER_H
#define _QUERYWRAPPERFILTER_H

#include "DocIdSet.h"

namespace Lucene {

class QueryWrapperFilterDocIdSet : public DocIdSet {
public:
    QueryWrapperFilterDocIdSet(const IndexReaderPtr& reader, const WeightPtr& weight);
    virtual ~QueryWrapperFilterDocIdSet();

    LUCENE_CLASS(QueryWrapperFilterDocIdSet);

protected:
    IndexReaderPtr reader;
    WeightPtr weight;

public:
    virtual DocIdSetIteratorPtr iterator();
    virtual bool isCacheable();
};

}

#endif
