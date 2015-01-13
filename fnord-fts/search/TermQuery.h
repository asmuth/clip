/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMQUERY_H
#define TERMQUERY_H

#include "fnord-fts/search/Query.h"

namespace Lucene {

/// A Query that matches documents containing a term.  This may be combined with other terms with a
/// {@link BooleanQuery}.
class TermQuery : public Query {
public:
    /// Constructs a query for the term.
    TermQuery(const TermPtr& term);

    virtual ~TermQuery();

    LUCENE_CLASS(TermQuery);

protected:
    TermPtr term;

public:
    using Query::toString;

    /// Returns the term of this query.
    TermPtr getTerm();

    virtual WeightPtr createWeight(const SearcherPtr& searcher);
    virtual void extractTerms(SetTerm terms);

    /// Prints a user-readable version of this query.
    virtual String toString(const String& field);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    friend class TermWeight;
};

}

#endif
