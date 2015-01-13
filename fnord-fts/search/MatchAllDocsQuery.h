/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MATCHALLDOCSQUERY_H
#define MATCHALLDOCSQUERY_H

#include "Query.h"

namespace Lucene {

/// A query that matches all documents.
class MatchAllDocsQuery : public Query {
public:
    /// @param normsField Field used for normalization factor (document boost). Null if nothing.
    MatchAllDocsQuery(const String& normsField = EmptyString);

    virtual ~MatchAllDocsQuery();

    LUCENE_CLASS(MatchAllDocsQuery);

protected:
    String normsField;

public:
    using Query::toString;

    virtual WeightPtr createWeight(const SearcherPtr& searcher);
    virtual void extractTerms(SetTerm terms);
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    friend class MatchAllDocsWeight;
};

}

#endif
