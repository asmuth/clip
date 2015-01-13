/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _BOOLEANQUERY_H
#define _BOOLEANQUERY_H

#include "SimilarityDelegator.h"

namespace Lucene {

/// The Weight for BooleanQuery, used to normalize, score and explain these queries.
class BooleanWeight : public Weight {
public:
    BooleanWeight(const BooleanQueryPtr& query, const SearcherPtr& searcher);
    virtual ~BooleanWeight();

    LUCENE_CLASS(BooleanWeight);

protected:
    BooleanQueryPtr query;

    /// The Similarity implementation.
    SimilarityPtr similarity;
    Collection<WeightPtr> weights;

public:
    virtual QueryPtr getQuery();
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
    virtual bool scoresDocsOutOfOrder();
};

/// Disabled coord Similarity
class SimilarityDisableCoord : public SimilarityDelegator {
public:
    SimilarityDisableCoord(const SimilarityPtr& delegee);
    virtual ~SimilarityDisableCoord();

    LUCENE_CLASS(SimilarityDisableCoord);

public:
    virtual double coord(int32_t overlap, int32_t maxOverlap);
};

}

#endif
