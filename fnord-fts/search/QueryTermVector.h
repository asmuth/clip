/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYTERMVECTOR_H
#define QUERYTERMVECTOR_H

#include "fnord-fts/index/TermFreqVector.h"

namespace fnord {
namespace fts {

class QueryTermVector : public TermFreqVector, public LuceneObject {
public:
    /// @param queryTerms The original list of terms from the query, can contain duplicates
    QueryTermVector(Collection<String> queryTerms);
    QueryTermVector(const String& queryString, const AnalyzerPtr& analyzer);

    virtual ~QueryTermVector();

    LUCENE_CLASS(QueryTermVector);

protected:
    Collection<String> terms;
    Collection<int32_t> termFreqs;

public:
    virtual String toString();

    int32_t size();
    Collection<String> getTerms();
    Collection<int32_t> getTermFrequencies();
    int32_t indexOf(const String& term);
    Collection<int32_t> indexesOf(Collection<String> terms, int32_t start, int32_t length);

protected:
    void processTerms(Collection<String> queryTerms);
};

}

}
#endif
