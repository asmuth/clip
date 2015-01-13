/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _TERMQUERY_H
#define _TERMQUERY_H

#include "Weight.h"

namespace Lucene {

class TermWeight : public Weight {
public:
    TermWeight(const TermQueryPtr& query, const SearcherPtr& searcher);
    virtual ~TermWeight();

    LUCENE_CLASS(TermWeight);

protected:
    TermQueryPtr query;
    SimilarityPtr similarity;
    double value;
    double idf;
    double queryNorm;
    double queryWeight;
    IDFExplanationPtr idfExp;

public:
    virtual String toString();
    virtual QueryPtr getQuery();
    virtual double getValue();
    virtual double sumOfSquaredWeights();
    virtual void normalize(double norm);
    virtual ScorerPtr scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer);
    virtual ExplanationPtr explain(const IndexReaderPtr& reader, int32_t doc);
};

}

#endif
