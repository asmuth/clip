/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CONJUNCTIONSCORER_H
#define CONJUNCTIONSCORER_H

#include "Scorer.h"

namespace Lucene {

/// Scorer for conjunctions, sets of queries, all of which are required.
class ConjunctionScorer : public Scorer {
public:
    ConjunctionScorer(const SimilarityPtr& similarity, Collection<ScorerPtr> scorers);
    virtual ~ConjunctionScorer();

    LUCENE_CLASS(ConjunctionScorer);

protected:
    Collection<ScorerPtr> scorers;
    double coord;
    int32_t lastDoc;

public:
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();

protected:
    int32_t doNext();
};

}

#endif
