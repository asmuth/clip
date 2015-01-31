/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANWEIGHT_H
#define SPANWEIGHT_H

#include "fnord-fts/search/Weight.h"

namespace fnord {
namespace fts {

/// Public for use by other weight implementations
class SpanWeight : public Weight {
public:
    SpanWeight(const SpanQueryPtr& query, const SearcherPtr& searcher);
    virtual ~SpanWeight();

    LUCENE_CLASS(SpanWeight);

protected:
    SimilarityPtr similarity;
    double value;
    double idf;
    double queryNorm;
    double queryWeight;

    SetTerm terms;
    SpanQueryPtr query;
    IDFExplanationPtr idfExp;

public:
    virtual QueryPtr getQuery();
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);

    friend class PayloadNearSpanScorer;
    friend class PayloadTermSpanScorer;
};

}

}
#endif
