/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELDVALUEHITQUEUE_H
#define _FIELDVALUEHITQUEUE_H

#include "LuceneObject.h"

namespace Lucene {

/// An implementation of {@link FieldValueHitQueue} which is optimized in case there is just one comparator.
class OneComparatorFieldValueHitQueue : public FieldValueHitQueue {
public:
    OneComparatorFieldValueHitQueue(Collection<SortFieldPtr> fields, int32_t size);
    virtual ~OneComparatorFieldValueHitQueue();

    LUCENE_CLASS(OneComparatorFieldValueHitQueue);

public:
    FieldComparatorPtr comparator;
    int32_t oneReverseMul;

protected:
    virtual bool lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second);
};

/// An implementation of {@link FieldValueHitQueue} which is optimized in case there is more than one comparator.
class MultiComparatorsFieldValueHitQueue : public FieldValueHitQueue {
public:
    MultiComparatorsFieldValueHitQueue(Collection<SortFieldPtr> fields, int32_t size);
    virtual ~MultiComparatorsFieldValueHitQueue();

    LUCENE_CLASS(MultiComparatorsFieldValueHitQueue);

protected:
    virtual bool lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second);
};

}

#endif
