/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILTEREDQUERY_H
#define _FILTEREDQUERY_H

#include "Weight.h"
#include "Scorer.h"

namespace Lucene {

class FilteredQueryWeight : public Weight {
public:
    FilteredQueryWeight(const FilteredQueryPtr& query, const WeightPtr& weight, const SimilarityPtr& similarity);
    virtual ~FilteredQueryWeight();

    LUCENE_CLASS(FilteredQueryWeight);

protected:
    FilteredQueryPtr query;
    WeightPtr weight;
    SimilarityPtr similarity;
    double value;

public:
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
    virtual QueryPtr getQuery();
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);

    friend class FilteredQueryWeightScorer;
};

class FilteredQueryWeightScorer : public Scorer {
public:
    FilteredQueryWeightScorer(const FilteredQueryWeightPtr& weight, const ScorerPtr& scorer, const DocIdSetIteratorPtr& docIdSetIterator, const SimilarityPtr& similarity);
    virtual ~FilteredQueryWeightScorer();

    LUCENE_CLASS(FilteredQueryWeightScorer);

protected:
    FilteredQueryWeightPtr weight;
    ScorerPtr scorer;
    DocIdSetIteratorPtr docIdSetIterator;
    int32_t doc;

public:
    virtual int32_t nextDoc();
    virtual int32_t docID();
    virtual int32_t advance(int32_t target);
    virtual double score();

protected:
    int32_t advanceToCommon(int32_t scorerDoc, int32_t disiDoc);
};

}

#endif
