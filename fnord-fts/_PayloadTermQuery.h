/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _PAYLOADTERMQUERY_H
#define _PAYLOADTERMQUERY_H

#include "SpanWeight.h"
#include "SpanScorer.h"

namespace Lucene {

class PayloadTermWeight : public SpanWeight {
public:
    PayloadTermWeight(const PayloadTermQueryPtr& query, const SearcherPtr& searcher);
    virtual ~PayloadTermWeight();

    LUCENE_CLASS(PayloadTermWeight);

public:
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
};

class PayloadTermSpanScorer : public SpanScorer {
public:
    PayloadTermSpanScorer(const TermSpansPtr& spans, const WeightPtr& weight, const SimilarityPtr& similarity, ByteArray norms);
    virtual ~PayloadTermSpanScorer();

    LUCENE_CLASS(PayloadTermSpanScorer);

protected:
    ByteArray payload;
    TermPositionsPtr positions;
    double payloadScore;
    int32_t payloadsSeen;

public:
    virtual double score();

protected:
    virtual bool setFreqCurrentDoc();

    void processPayload(const SimilarityPtr& similarity);

    /// Returns the SpanScorer score only.
    ///
    /// Should not be overridden without good cause
    ///
    /// @return the score for just the Span part without the payload
    /// @see #score()
    virtual double getSpanScore();

    /// The score for the payload
    ///
    /// @return The score, as calculated by {@link PayloadFunction#docScore(int32_t, const String&,
    /// int32_t, double)}
    virtual double getPayloadScore();

    virtual ExplanationPtr explain(int32_t doc);
};

}

#endif
