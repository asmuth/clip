/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PAYLOADNEARQUERY_H
#define PAYLOADNEARQUERY_H

#include "fnord-fts/search/spans/SpanNearQuery.h"
#include "fnord-fts/search/spans/SpanWeight.h"
#include "fnord-fts/search/spans/SpanScorer.h"

namespace fnord {
namespace fts {

/// This class is very similar to {@link SpanNearQuery} except that it factors in the value of the payloads
/// located at each of the positions where the {@link TermSpans} occurs.
///
/// In order to take advantage of this, you must override {@link Similarity#scorePayload} which returns 1
/// by default.
///
/// Payload scores are aggregated using a pluggable {@link PayloadFunction}.
///
/// @see Similarity#scorePayload
class PayloadNearQuery : public SpanNearQuery {
public:
    PayloadNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder);
    PayloadNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder, const PayloadFunctionPtr& function);

    virtual ~PayloadNearQuery();

    LUCENE_CLASS(PayloadNearQuery);

protected:
    String fieldName;
    PayloadFunctionPtr function;

public:
    using SpanNearQuery::toString;

    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    friend class PayloadNearSpanWeight;
    friend class PayloadNearSpanScorer;
};

class PayloadNearSpanWeight : public SpanWeight {
public:
    PayloadNearSpanWeight(const SpanQueryPtr& query, const SearcherPtr& searcher);
    virtual ~PayloadNearSpanWeight();

    LUCENE_CLASS(PayloadNearSpanWeight);

public:
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
};

class PayloadNearSpanScorer : public SpanScorer {
public:
    PayloadNearSpanScorer(const SpansPtr& spans, const WeightPtr& weight, const SimilarityPtr& similarity, ByteArray norms);
    virtual ~PayloadNearSpanScorer();

    LUCENE_CLASS(PayloadNearSpanScorer);

public:
    SpansPtr spans;
    SimilarityPtr similarity;

protected:
    double payloadScore;
    int32_t payloadsSeen;

public:
    /// Get the payloads associated with all underlying subspans
    void getPayloads(Collection<SpansPtr> subSpans);

    virtual double score();

protected:
    /// By default, uses the {@link PayloadFunction} to score the payloads, but can be overridden to do
    /// other things.
    /// @param payLoads The payloads
    /// @param start The start position of the span being scored
    /// @param end The end position of the span being scored
    /// @see Spans
    void processPayloads(Collection<ByteArray> payLoads, int32_t start, int32_t end);

    virtual bool setFreqCurrentDoc();
    virtual ExplanationPtr explain(int32_t doc);
};

}

}
#endif
