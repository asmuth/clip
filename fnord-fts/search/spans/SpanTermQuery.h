/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANTERMQUERY_H
#define SPANTERMQUERY_H

#include "fnord-fts/search/spans/SpanQuery.h"

namespace Lucene {

/// Matches spans containing a term.
class SpanTermQuery : public SpanQuery {
public:
    /// Construct a SpanTermQuery matching the named term's spans.
    SpanTermQuery(const TermPtr& term);
    virtual ~SpanTermQuery();

    LUCENE_CLASS(SpanTermQuery);

protected:
    TermPtr term;

public:
    using SpanQuery::toString;

    /// Return the term whose spans are matched.
    TermPtr getTerm();

    virtual String getField();
    virtual void extractTerms(SetTerm terms);
    virtual String toString(const String& field);
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);
};

}

#endif
