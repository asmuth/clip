/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANORQUERY_H
#define SPANORQUERY_H

#include "fnord-fts/search/spans/SpanQuery.h"

namespace fnord {
namespace fts {

/// Matches the union of its clauses.
class SpanOrQuery : public SpanQuery {
public:
    /// Construct a SpanOrQuery merging the provided clauses.
    SpanOrQuery(Collection<SpanQueryPtr> clauses);
    virtual ~SpanOrQuery();

    LUCENE_CLASS(SpanOrQuery);

protected:
    Collection<SpanQueryPtr> clauses;
    String field;

public:
    using SpanQuery::toString;

    /// Return the clauses whose spans are matched.
    Collection<SpanQueryPtr> getClauses();

    virtual String getField();
    virtual void extractTerms(SetTerm terms);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);

    friend class OrSpans;
};

}

}
#endif
