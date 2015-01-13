/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MULTIPHRASEQUERY_H
#define _MULTIPHRASEQUERY_H

#include "Weight.h"

namespace Lucene {

class MultiPhraseWeight : public Weight {
public:
    MultiPhraseWeight(const MultiPhraseQueryPtr& query, const SearcherPtr& searcher);
    virtual ~MultiPhraseWeight();

    LUCENE_CLASS(MultiPhraseWeight);

protected:
    MultiPhraseQueryPtr query;
    SimilarityPtr similarity;
    double value;
    double idf;
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

}

#endif
