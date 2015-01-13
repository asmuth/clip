/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHT_H
#define WEIGHT_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Calculate query weights and build query scorers.
///
/// The purpose of {@link Weight} is to ensure searching does not modify a {@link Query}, so that a
/// {@link Query} instance can be reused.
/// {@link Searcher} dependent state of the query should reside in the {@link Weight}.
/// {@link IndexReader} dependent state should reside in the {@link Scorer}.
///
/// Weight is used in the following way:
/// <ol>
/// <li>A Weight is constructed by a top-level query, given a Searcher ({@link Query#createWeight(Searcher)}).
/// <li>The {@link #sumOfSquaredWeights()} method is called on the Weight to compute the query normalization
/// factor {@link Similarity#queryNorm(float)} of the query clauses contained in the query.
/// <li>The query normalization factor is passed to {@link #normalize(float)}. At this point the weighting is
/// complete.
/// <li>A Scorer is constructed by {@link #scorer(IndexReaderPtr, bool, bool)}.
/// </ol>
class Weight : public LuceneObject {
public:
    virtual ~Weight();
    LUCENE_CLASS(Weight);

public:
    /// An explanation of the score computation for the named document.
    /// @param reader sub-reader containing the give doc
    /// @param doc
    /// @return an Explanation for the score
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc) = 0;

    /// The query that this concerns.
    virtual QueryPtr getQuery() = 0;

    /// The weight for this query.
    virtual double getValue() = 0;

    /// Assigns the query normalization factor to this.
    virtual void normalize(double norm) = 0;

    /// Returns a {@link Scorer} which scores documents in/out-of order according to scoreDocsInOrder.
    ///
    /// NOTE: even if scoreDocsInOrder is false, it is recommended to check whether the returned Scorer
    /// indeed scores documents out of order (ie., call {@link #scoresDocsOutOfOrder()}), as some Scorer
    /// implementations will always return documents in-order.
    ///
    /// NOTE: null can be returned if no documents will be scored by this query.
    ///
    /// @param reader The {@link IndexReader} for which to return the {@link Scorer}.
    /// @param scoreDocsInOrder Specifies whether in-order scoring of documents is required.  Note that if
    /// set to false (i.e., out-of-order scoring is required), this method can return whatever scoring mode
    /// it supports, as every in-order scorer is also an out-of-order one.  However, an out-of-order scorer
    /// may not support {@link Scorer#nextDoc()} and/or {@link Scorer#advance(int)}, therefore it is
    /// recommended to request an in-order scorer if use of these methods is required.
    /// @param topScorer If true, {@link Scorer#score(CollectorPtr)} will be called; if false, {@link
    /// Scorer#nextDoc()} and/or {@link Scorer#advance(int)} will be called.
    /// @return a {@link Scorer} which scores documents in/out-of order.
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) = 0;

    /// The sum of squared weights of contained query clauses.
    virtual double sumOfSquaredWeights() = 0;

    /// Returns true if this implementation scores docs only out of order.  This method is used in conjunction
    /// with {@link Collector}'s {@link Collector#acceptsDocsOutOfOrder() acceptsDocsOutOfOrder} and
    /// {@link #scorer(IndexReaderPtr, bool, bool)} to create a matching {@link Scorer} instance for a given
    /// {@link Collector}, or vice versa.
    ///
    /// NOTE: the default implementation returns false, ie. the Scorer scores documents in-order.
    virtual bool scoresDocsOutOfOrder();
};

}

#endif
