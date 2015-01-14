/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANNEARQUERY_H
#define SPANNEARQUERY_H

#include "fnord-fts/search/spans/SpanQuery.h"

namespace fnord {
namespace fts {

/// Matches spans which are near one another.  One can specify slop, the maximum number of intervening
/// unmatched positions, as well as whether matches are required to be in-order.
class SpanNearQuery : public SpanQuery {
public:
    /// Construct a SpanNearQuery.  Matches spans matching a span from each clause, with up to slop total
    /// unmatched positions between them.  * When inOrder is true, the spans from each clause must be
    /// ordered as in clauses.
    SpanNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder, bool collectPayloads = true);
    virtual ~SpanNearQuery();

    LUCENE_CLASS(SpanNearQuery);

protected:
    Collection<SpanQueryPtr> clauses;
    int32_t slop;
    bool inOrder;

    String field;
    bool collectPayloads;

public:
    using SpanQuery::toString;

    /// Return the clauses whose spans are matched.
    Collection<SpanQueryPtr> getClauses();

    /// Return the maximum number of intervening unmatched positions permitted.
    int32_t getSlop();

    /// Return true if matches are required to be in-order.
    bool isInOrder();

    virtual String getField();
    virtual void extractTerms(SetTerm terms);
    virtual String toString(const String& field);
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

}
#endif
