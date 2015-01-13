/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef REQEXCLSCORER_H
#define REQEXCLSCORER_H

#include "Scorer.h"

namespace Lucene {

/// A Scorer for queries with a required subscorer and an excluding (prohibited) sub DocIdSetIterator.
/// This Scorer implements {@link Scorer#skipTo(int32_t)}, and it uses the skipTo() on the given scorers.
class ReqExclScorer : public Scorer {
public:
    /// Construct a ReqExclScorer.
    /// @param reqScorer The scorer that must match, except where
    /// @param exclDisi indicates exclusion.
    ReqExclScorer(const ScorerPtr& reqScorer, const DocIdSetIteratorPtr& exclDisi);
    virtual ~ReqExclScorer();

    LUCENE_CLASS(ReqExclScorer);

protected:
    ScorerPtr reqScorer;
    DocIdSetIteratorPtr exclDisi;
    int32_t doc;

public:
    virtual int32_t nextDoc();
    virtual int32_t docID();

    /// Returns the score of the current document matching the query.  Initially invalid, until {@link #next()}
    /// is called the first time.
    /// @return The score of the required scorer.
    virtual double score();

    virtual int32_t advance(int32_t target);

protected:
    /// Advance to non excluded doc.
    ///
    /// On entry:
    /// <ul>
    /// <li>reqScorer != null,
    /// <li>exclScorer != null,
    /// <li>reqScorer was advanced once via next() or skipTo() and reqScorer.doc() may still be excluded.
    /// </ul>
    /// Advances reqScorer a non excluded required doc, if any.
    /// @return true iff there is a non excluded required doc.
    int32_t toNonExcluded();
};

}

#endif
