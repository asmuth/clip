/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SCORER_H
#define SCORER_H

#include "fnord-fts/search/DocIdSetIterator.h"

namespace fnord {
namespace fts {

/// Common scoring functionality for different types of queries.
///
/// A Scorer iterates over documents matching a query in increasing order of doc Id.
///
/// Document scores are computed using a given Similarity implementation.
///
/// NOTE: The values NEGATIVE_INFINITY and POSITIVE_INFINITY are not valid scores.  Certain collectors
/// (eg {@link TopScoreDocCollector}) will not properly collect hits with these scores.
class Scorer : public DocIdSetIterator {
public:
    /// Constructs a Scorer.
    /// @param similarity The Similarity implementation used by this scorer.
    Scorer(const SimilarityPtr& similarity);
    virtual ~Scorer();

    LUCENE_CLASS(Scorer);

protected:
    SimilarityPtr similarity;

public:
    /// Returns the Similarity implementation used by this scorer.
    SimilarityPtr getSimilarity();

    /// Scores and collects all matching documents.
    /// @param collector The collector to which all matching documents are passed.
    virtual void score(const CollectorPtr& collector);

    /// Returns the score of the current document matching the query.  Initially invalid, until {@link
    /// #nextDoc()} or {@link #advance(int32_t)} is called the first time, or when called from within
    /// {@link Collector#collect}.
    virtual double score() = 0;

protected:
    /// Collects matching documents in a range.  Hook for optimization.
    /// Note, firstDocID is added to ensure that {@link #nextDoc()} was called before this method.
    ///
    /// @param collector The collector to which all matching documents are passed.
    /// @param max Do not score documents past this.
    /// @param firstDocID The first document ID (ensures {@link #nextDoc()} is called before this method.
    /// @return true if more matching documents may remain.
    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);

    friend class BooleanScorer;
    friend class ScoreCachingWrappingScorer;
};

}

}
#endif
