/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _DISJUNCTIONMAXQUERY_H
#define _DISJUNCTIONMAXQUERY_H

#include "Weight.h"

namespace Lucene {

/// The Weight for DisjunctionMaxQuery, used to normalize, score and explain these queries.
class DisjunctionMaxWeight : public Weight {
public:
    /// Construct the Weight for this Query searched by searcher.  Recursively construct subquery weights.
    DisjunctionMaxWeight(const DisjunctionMaxQueryPtr& query, const SearcherPtr& searcher);
    virtual ~DisjunctionMaxWeight();

    LUCENE_CLASS(DisjunctionMaxWeight);

protected:
    DisjunctionMaxQueryPtr query;

    /// The Similarity implementation.
    SimilarityPtr similarity;

    /// The Weights for our subqueries, in 1-1 correspondence with disjuncts
    Collection<WeightPtr> weights;

public:
    /// Return our associated DisjunctionMaxQuery
    virtual QueryPtr getQuery();

    /// Return our boost
    virtual double getValue();

    /// Compute the sub of squared weights of us applied to our subqueries.  Used for normalization.
    virtual double sumOfSquaredWeights();

    /// Apply the computed normalization factor to our subqueries
    virtual void normalize(double norm);

    /// Create the scorer used to score our associated DisjunctionMaxQuery
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);

    /// Explain the score we computed for doc
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
};

}

#endif
