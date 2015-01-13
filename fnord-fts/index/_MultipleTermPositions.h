/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MULTIPLETERMPOSITIONS_H
#define _MULTIPLETERMPOSITIONS_H

#include "PriorityQueue.h"

namespace Lucene {

class TermPositionsQueue : public PriorityQueue<TermPositionsPtr> {
public:
    TermPositionsQueue(Collection<TermPositionsPtr> termPositions);
    virtual ~TermPositionsQueue();

    LUCENE_CLASS(TermPositionsQueue);

protected:
    Collection<TermPositionsPtr> termPositions;

public:
    virtual void initialize();

protected:
    virtual bool lessThan(const TermPositionsPtr& first, const TermPositionsPtr& second);
};

class IntQueue : public LuceneObject {
public:
    IntQueue();
    virtual ~IntQueue();

    LUCENE_CLASS(IntQueue);

protected:
    int32_t arraySize;
    int32_t index;
    int32_t lastIndex;
    Collection<int32_t> array;

public:
    void add(int32_t i);
    int32_t next();
    void sort();
    void clear();
    int32_t size();

protected:
    void growArray();
};

}

#endif
