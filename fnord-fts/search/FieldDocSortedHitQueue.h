/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDDOCSORTEDHITQUEUE_H
#define FIELDDOCSORTEDHITQUEUE_H

#include "PriorityQueue.h"

namespace Lucene {

/// Collects sorted results from Searchable's and collates them.
/// The elements put into this queue must be of type FieldDoc.
class FieldDocSortedHitQueue : public PriorityQueue<FieldDocPtr> {
public:
    FieldDocSortedHitQueue(int32_t size);
    virtual ~FieldDocSortedHitQueue();

    LUCENE_CLASS(FieldDocSortedHitQueue);

public:
    Collection<SortFieldPtr> fields;

    // used in the case where the fields are sorted by locale based strings
    Collection<CollatorPtr> collators;

public:
    /// Allows redefinition of sort fields if they are null.  This is to handle the case using
    /// ParallelMultiSearcher where the original list contains AUTO and we don't know the actual sort
    /// type until the values come back.  The fields can only be set once.  This method should be
    /// synchronized external like all other PQ methods.
    void setFields(Collection<SortFieldPtr> fields);

    /// Returns the fields being used to sort.
    Collection<SortFieldPtr> getFields();

protected:
    /// Returns an array of collators, possibly null.  The collators correspond to any SortFields which
    /// were given a specific locale.
    /// @param fields Array of sort fields.
    /// @return Array, possibly null.
    Collection<CollatorPtr> hasCollators(Collection<SortFieldPtr> fields);

    /// Returns whether first is less relevant than second.
    virtual bool lessThan(const FieldDocPtr& first, const FieldDocPtr& second);
};

}

#endif
