/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SCORERDOCQUEUE_H
#define SCORERDOCQUEUE_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// A ScorerDocQueue maintains a partial ordering of its Scorers such that the least Scorer can always be
/// found in constant time.  Put()'s and pop()'s require log(size) time.  The ordering is by Scorer::doc().
class ScorerDocQueue : public LuceneObject {
public:
    ScorerDocQueue(int32_t maxSize);
    virtual ~ScorerDocQueue();

    LUCENE_CLASS(ScorerDocQueue);

protected:
    Collection<HeapedScorerDocPtr> heap;
    int32_t maxSize;
    int32_t _size;
    HeapedScorerDocPtr topHSD; // same as heap[1], only for speed

public:
    /// Adds a Scorer to a ScorerDocQueue in log(size) time.  If one tries to add more Scorers than maxSize
    /// ArrayIndexOutOfBound exception is thrown.
    void put(const ScorerPtr& scorer);

    /// Adds a Scorer to the ScorerDocQueue in log(size) time if either the ScorerDocQueue is not full, or
    /// not lessThan(scorer, top()).
    /// @return true if scorer is added, false otherwise.
    bool insert(const ScorerPtr& scorer);

    /// Returns the least Scorer of the ScorerDocQueue in constant time. Should not be used when the queue
    /// is empty.
    ScorerPtr top();

    /// Returns document number of the least Scorer of the ScorerDocQueue in constant time.
    /// Should not be used when the queue is empty.
    int32_t topDoc();

    double topScore();
    bool topNextAndAdjustElsePop();
    bool topSkipToAndAdjustElsePop(int32_t target);

    /// Removes and returns the least scorer of the ScorerDocQueue in log(size) time.  Should not be used
    /// when the queue is empty.
    ScorerPtr pop();

    /// Should be called when the scorer at top changes doc() value.
    void adjustTop();

    /// Returns the number of scorers currently stored in the ScorerDocQueue.
    int32_t size();

    /// Removes all entries from the ScorerDocQueue.
    void clear();

protected:
    bool checkAdjustElsePop(bool cond);

    /// Removes the least scorer of the ScorerDocQueue in log(size) time.  Should not be used when the
    /// queue is empty.
    void popNoResult();

    void upHeap();
    void downHeap();
};

}

#endif
