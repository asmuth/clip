/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANFIRSTQUERY_H
#define SPANFIRSTQUERY_H

#include "SpanQuery.h"
#include "Spans.h"

namespace Lucene {

/// Matches spans near the beginning of a field.
class SpanFirstQuery : public SpanQuery {
public:
    /// Construct a SpanFirstQuery matching spans in match whose end position is less than or equal to end.
    SpanFirstQuery(const SpanQueryPtr& match, int32_t end);
    virtual ~SpanFirstQuery();

    LUCENE_CLASS(SpanFirstQuery);

protected:
    SpanQueryPtr match;
    int32_t end;

public:
    using SpanQuery::toString;

    /// Return the SpanQuery whose matches are filtered.
    SpanQueryPtr getMatch();

    /// Return the maximum end position permitted in a match.
    int32_t getEnd();

    virtual String getField();
    virtual String toString(const String& field);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual void extractTerms(SetTerm terms);
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    friend class FirstSpans;
};

}

#endif
