/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANNOTQUERY_H
#define SPANNOTQUERY_H

#include "fnord-fts/search/spans/SpanQuery.h"

namespace fnord {
namespace fts {

/// Removes matches which overlap with another SpanQuery.
class SpanNotQuery : public SpanQuery {
public:
    /// Construct a SpanNotQuery matching spans from include which have no overlap with spans from exclude.
    SpanNotQuery(const SpanQueryPtr& include, const SpanQueryPtr& exclude);
    virtual ~SpanNotQuery();

    LUCENE_CLASS(SpanNotQuery);

protected:
    SpanQueryPtr include;
    SpanQueryPtr exclude;

public:
    using SpanQuery::toString;

    /// Return the SpanQuery whose matches are filtered.
    SpanQueryPtr getInclude();

    /// Return the SpanQuery whose matches must not overlap those returned.
    SpanQueryPtr getExclude();

    virtual String getField();
    virtual void extractTerms(SetTerm terms);
    virtual String toString(const String& field);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

}
#endif
