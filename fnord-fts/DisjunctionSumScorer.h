/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DISJUNCTIONSUMSCORER_H
#define DISJUNCTIONSUMSCORER_H

#include "Scorer.h"

namespace Lucene {

/// A Scorer for OR like queries, counterpart of ConjunctionScorer.  This Scorer implements {@link
/// Scorer#skipTo(int32_t)} and uses skipTo() on the given Scorers.
class DisjunctionSumScorer : public Scorer {
public:
    DisjunctionSumScorer(Collection<ScorerPtr> subScorers, int32_t minimumNrMatchers = 1);
    virtual ~DisjunctionSumScorer();

    LUCENE_CLASS(DisjunctionSumScorer);

protected:
    /// The number of subscorers.
    int32_t nrScorers;

    /// The subscorers.
    Collection<ScorerPtr> subScorers;

    /// The minimum number of scorers that should match.
    int32_t minimumNrMatchers;

    /// The scorerDocQueue contains all subscorers ordered by their current doc(), with the minimum at
    /// the top.  The scorerDocQueue is initialized the first time next() or skipTo() is called.  An exhausted
    /// scorer is immediately removed from the scorerDocQueue.  If less than the minimumNrMatchers scorers
    /// remain in the scorerDocQueue next() and skipTo() return false.
    ///
    /// After each to call to next() or skipTo() currentSumScore is the total score of the current matching doc,
    /// nrMatchers is the number of matching scorers, and all scorers are after the matching doc, or are exhausted.
    ScorerDocQueuePtr scorerDocQueue;

    /// The document number of the current match.
    int32_t currentDoc;

    /// The number of subscorers that provide the current match.
    int32_t _nrMatchers;

    double currentScore;

public:
    virtual void initialize();

    virtual void score(const CollectorPtr& collector);
    virtual int32_t nextDoc();

    /// Returns the score of the current document matching the query. Initially invalid, until {@link #next()}
    /// is called the first time.
    virtual double score();

    virtual int32_t docID();

    /// Returns the number of subscorers matching the current document.  Initially invalid, until {@link #next()}
    /// is called the first time.
    int32_t nrMatchers();

    /// Advances to the first match beyond the current whose document number is greater than or equal to a given
    /// target.  The implementation uses the skipTo() method on the subscorers.
    ///
    /// @param target The target document number.
    /// @return the document whose number is greater than or equal to the given target, or -1 if none exist.
    virtual int32_t advance(int32_t target);

protected:
    /// Called the first time next() or skipTo() is called to initialize scorerDocQueue.
    void initScorerDocQueue();

    /// Collects matching documents in a range.  Hook for optimization.  Note that {@link #next()} must be
    /// called once before this method is called for the first time.
    /// @param collector The collector to which all matching documents are passed through.
    /// @param max Do not score documents past this.
    /// @return true if more matching documents may remain.
    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);

    /// Advance all subscorers after the current document determined by the top of the scorerDocQueue.  Repeat
    /// until at least the minimum number of subscorers match on the same document and all subscorers are after
    /// that document or are exhausted.  On entry the scorerDocQueue has at least minimumNrMatchers available.
    /// At least the scorer with the minimum document number will be advanced.
    /// @return true if there is a match.  In case there is a match, currentDoc, currentSumScore and nrMatchers
    /// describe the match.
    bool advanceAfterCurrent();
};

}

#endif
