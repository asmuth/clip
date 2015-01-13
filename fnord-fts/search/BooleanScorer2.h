/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOLEANSCORER2_H
#define BOOLEANSCORER2_H

#include "DisjunctionSumScorer.h"
#include "ConjunctionScorer.h"

namespace Lucene {

/// See the description in BooleanScorer, comparing BooleanScorer & BooleanScorer2
///
/// An alternative to BooleanScorer that also allows a minimum number of optional scorers that should match.
/// Implements skipTo(), and has no limitations on the numbers of added scorers.
/// Uses ConjunctionScorer, DisjunctionScorer, ReqOptScorer and ReqExclScorer.
class BooleanScorer2 : public Scorer {
public:
    /// Creates a {@link Scorer} with the given similarity and lists of required, prohibited and optional
    /// scorers. In no required scorers are added, at least one of the optional scorers will have to match
    /// during the search.
    ///
    /// @param similarity The similarity to be used.
    /// @param minNrShouldMatch The minimum number of optional added scorers that should match during the search.
    /// In case no required scorers are added, at least one of the optional scorers will have to match during
    /// the search.
    /// @param required The list of required scorers.
    /// @param prohibited The list of prohibited scorers.
    /// @param optional The list of optional scorers.
    BooleanScorer2(const SimilarityPtr& similarity, int32_t minNrShouldMatch, Collection<ScorerPtr> required, Collection<ScorerPtr> prohibited, Collection<ScorerPtr> optional);

    virtual ~BooleanScorer2();

    LUCENE_CLASS(BooleanScorer2);

protected:
    Collection<ScorerPtr> requiredScorers;
    Collection<ScorerPtr> optionalScorers;
    Collection<ScorerPtr> prohibitedScorers;

    CoordinatorPtr coordinator;

    /// The scorer to which all scoring will be delegated, except for computing and using the coordination factor.
    ScorerPtr countingSumScorer;

    int32_t minNrShouldMatch;
    int32_t doc;

public:
    virtual void initialize();

    /// Scores and collects all matching documents.
    /// @param collector The collector to which all matching documents are passed through.
    virtual void score(const CollectorPtr& collector);

    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();
    virtual int32_t advance(int32_t target);

protected:
    ScorerPtr countingDisjunctionSumScorer(Collection<ScorerPtr> scorers, int32_t minNrShouldMatch);
    ScorerPtr countingConjunctionSumScorer(Collection<ScorerPtr> requiredScorers);
    ScorerPtr dualConjunctionSumScorer(const ScorerPtr& req1, const ScorerPtr& req2);

    /// Returns the scorer to be used for match counting and score summing.  Uses requiredScorers, optionalScorers
    /// and prohibitedScorers.
    ScorerPtr makeCountingSumScorer();
    ScorerPtr makeCountingSumScorerNoReq();
    ScorerPtr makeCountingSumScorerSomeReq();

    /// Returns the scorer to be used for match counting and score summing.  Uses the given required scorer and
    /// the prohibitedScorers.
    /// @param requiredCountingSumScorer A required scorer already built.
    ScorerPtr addProhibitedScorers(const ScorerPtr& requiredCountingSumScorer);

    friend class CountingDisjunctionSumScorer;
    friend class CountingConjunctionSumScorer;
};

class Coordinator : public LuceneObject {
public:
    Coordinator(const BooleanScorer2Ptr& scorer);
    virtual ~Coordinator();

    LUCENE_CLASS(Coordinator);

public:
    BooleanScorer2WeakPtr _scorer;
    Collection<double> coordFactors;
    int32_t maxCoord; // to be increased for each non prohibited scorer
    int32_t nrMatchers; // to be increased by score() of match counting scorers.

public:
    void init(); // use after all scorers have been added.

    friend class BooleanScorer2;
};

/// Count a scorer as a single match.
class SingleMatchScorer : public Scorer {
public:
    SingleMatchScorer(const ScorerPtr& scorer, const CoordinatorPtr& coordinator);
    virtual ~SingleMatchScorer();

    LUCENE_CLASS(SingleMatchScorer);

protected:
    ScorerPtr scorer;
    CoordinatorPtr coordinator;
    int32_t lastScoredDoc;
    double lastDocScore;

public:
    virtual double score();
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
};

class CountingDisjunctionSumScorer : public DisjunctionSumScorer {
public:
    CountingDisjunctionSumScorer(const BooleanScorer2Ptr& scorer, Collection<ScorerPtr> subScorers, int32_t minimumNrMatchers);
    virtual ~CountingDisjunctionSumScorer();

    LUCENE_CLASS(CountingDisjunctionSumScorer);

protected:
    BooleanScorer2WeakPtr _scorer;
    int32_t lastScoredDoc;

    // Save the score of lastScoredDoc, so that we don't compute it more than once in score().
    double lastDocScore;

public:
    virtual double score();

    friend class BooleanScorer2;
};

class CountingConjunctionSumScorer : public ConjunctionScorer {
public:
    CountingConjunctionSumScorer(const BooleanScorer2Ptr& scorer, const SimilarityPtr& similarity, Collection<ScorerPtr> scorers);
    virtual ~CountingConjunctionSumScorer();

    LUCENE_CLASS(CountingConjunctionSumScorer);

protected:
    BooleanScorer2WeakPtr _scorer;
    int32_t lastScoredDoc;
    int32_t requiredNrMatchers;

    // Save the score of lastScoredDoc, so that we don't compute it more than once in score().
    double lastDocScore;

public:
    virtual double score();
};

}

#endif
