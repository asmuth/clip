/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MATCHALLDOCSQUERY_H
#define _MATCHALLDOCSQUERY_H

#include "Weight.h"
#include "Scorer.h"

namespace Lucene {

class MatchAllDocsWeight : public Weight {
public:
    MatchAllDocsWeight(const MatchAllDocsQueryPtr& query, const SearcherPtr& searcher);
    virtual ~MatchAllDocsWeight();

    LUCENE_CLASS(MatchAllDocsWeight);

protected:
    MatchAllDocsQueryPtr query;
    SimilarityPtr similarity;
    double queryWeight;
    double queryNorm;

public:
    virtual String toString();
    virtual QueryPtr getQuery();
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
};

class MatchAllScorer : public Scorer {
public:
    MatchAllScorer(const MatchAllDocsQueryPtr& query, const IndexReaderPtr& reader, const SimilarityPtr& similarity, const WeightPtr& weight, ByteArray norms);
    virtual ~MatchAllScorer();

    LUCENE_CLASS(MatchAllScorer);

public:
    TermDocsPtr termDocs;
    double _score;
    ByteArray norms;

protected:
    MatchAllDocsQueryPtr query;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();
    virtual int32_t advance(int32_t target);
};

}

#endif
