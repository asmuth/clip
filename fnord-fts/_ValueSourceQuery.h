/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _VALUESOURCEQUERY_H
#define _VALUESOURCEQUERY_H

#include "Weight.h"
#include "Scorer.h"

namespace Lucene {

class ValueSourceWeight : public Weight {
public:
    ValueSourceWeight(const ValueSourceQueryPtr& query, const SearcherPtr& searcher);
    virtual ~ValueSourceWeight();

    LUCENE_CLASS(ValueSourceWeight);

public:
    ValueSourceQueryPtr query;
    SimilarityPtr similarity;
    double queryNorm;
    double queryWeight;

public:
    virtual QueryPtr getQuery();
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
};

/// A scorer that (simply) matches all documents, and scores each document with the value of the value
/// source in effect. As an example, if the value source is a (cached) field source, then value of that
/// field in that document will be used. (assuming field is indexed for this doc, with a single token.)
class ValueSourceScorer : public Scorer {
public:
    ValueSourceScorer(const SimilarityPtr& similarity, const IndexReaderPtr& reader, const ValueSourceWeightPtr& weight);
    virtual ~ValueSourceScorer();

    LUCENE_CLASS(ValueSourceScorer);

public:
    ValueSourceWeightPtr weight;
    double qWeight;
    DocValuesPtr vals;
    TermDocsPtr termDocs;
    int32_t doc;

public:
    virtual int32_t nextDoc();
    virtual int32_t docID();
    virtual int32_t advance(int32_t target);
    virtual double score();
};

}

#endif
