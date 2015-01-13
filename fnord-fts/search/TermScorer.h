/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSCORER_H
#define TERMSCORER_H

#include "Scorer.h"

namespace Lucene {

/// A Scorer for documents matching a Term.
class TermScorer : public Scorer {
public:
    /// Construct a TermScorer.
    /// @param weight The weight of the Term in the query.
    /// @param td An iterator over the documents matching the Term.
    /// @param similarity The Similarity implementation to be used for score computations.
    /// @param norms The field norms of the document fields for the Term.
    TermScorer(const WeightPtr& weight, const TermDocsPtr& td, const SimilarityPtr& similarity, ByteArray norms);

    virtual ~TermScorer();

    LUCENE_CLASS(TermScorer);

protected:
    WeightPtr weight;
    TermDocsPtr termDocs;
    ByteArray norms;
    double weightValue;
    int32_t doc;

    Collection<int32_t> docs; // buffered doc numbers
    Collection<int32_t> freqs; // buffered term freqs
    int32_t pointer;
    int32_t pointerMax;

    static const int32_t SCORE_CACHE_SIZE;
    Collection<double> scoreCache;

public:
    virtual void score(const CollectorPtr& collector);
    virtual int32_t docID();

    /// Advances to the next document matching the query.
    /// The iterator over the matching documents is buffered using {@link
    /// TermDocs#read(Collection<int32_t>, Collection<int32_t>)}.
    /// @return the document matching the query or -1 if there are no more documents.
    virtual int32_t nextDoc();

    virtual double score();

    /// Advances to the first match beyond the current whose document number is greater than or equal to a
    /// given target.  The implementation uses {@link TermDocs#skipTo(int32_t)}.
    /// @param target The target document number.
    /// @return the matching document or -1 if none exist.
    virtual int32_t advance(int32_t target);

    /// Returns a string representation of this TermScorer.
    virtual String toString();

protected:
    static const Collection<double> SIM_NORM_DECODER();

    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);
};

}

#endif
