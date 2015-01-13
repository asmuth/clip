/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FUZZYQUERY_H
#define _FUZZYQUERY_H

#include "PriorityQueue.h"

namespace Lucene {

class ScoreTerm : public LuceneObject {
public:
    virtual ~ScoreTerm();
    LUCENE_CLASS(ScoreTerm);

public:
    TermPtr term;
    double score;

public:
    int32_t compareTo(const ScoreTermPtr& other);
};

class ScoreTermQueue : public PriorityQueue<ScoreTermPtr> {
public:
    ScoreTermQueue(int32_t size);
    virtual ~ScoreTermQueue();

    LUCENE_CLASS(ScoreTermQueue);

protected:
    virtual bool lessThan(const ScoreTermPtr& first, const ScoreTermPtr& second);
};

}

#endif
