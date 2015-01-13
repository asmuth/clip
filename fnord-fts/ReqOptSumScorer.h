/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef REQOPTSUMSCORER_H
#define REQOPTSUMSCORER_H

#include "Scorer.h"

namespace Lucene {

/// A Scorer for queries with a required part and an optional part.  Delays skipTo() on the optional part
/// until a score() is needed.  This Scorer implements {@link Scorer#skipTo(int32_t)}.
class ReqOptSumScorer : public Scorer {
public:
    ReqOptSumScorer(const ScorerPtr& reqScorer, const ScorerPtr& optScorer);
    virtual ~ReqOptSumScorer();

    LUCENE_CLASS(ReqOptSumScorer);

protected:
    ScorerPtr reqScorer;
    ScorerPtr optScorer;

public:
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();

    /// Returns the score of the current document matching the query.  Initially invalid, until {@link #next()}
    /// is called the first time.
    /// @return The score of the required scorer, eventually increased by the score of the optional scorer when
    /// it also matches the current document.
    virtual double score();
};

}

#endif
