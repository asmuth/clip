/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANTSCOREQUERY_H
#define CONSTANTSCOREQUERY_H

#include "fnord-fts/search/Query.h"
#include "fnord-fts/search/Weight.h"
#include "fnord-fts/search/Scorer.h"

namespace fnord {
namespace fts {

/// A query that wraps a filter and simply returns a constant score equal to the query boost for every
/// document in the filter.
class ConstantScoreQuery : public Query {
public:
    ConstantScoreQuery(const FilterPtr& filter);
    virtual ~ConstantScoreQuery();

    LUCENE_CLASS(ConstantScoreQuery);

protected:
    FilterPtr filter;

public:
    using Query::toString;

    /// Returns the encapsulated filter
    FilterPtr getFilter();

    virtual QueryPtr rewrite(const IndexReaderPtr& reader);
    virtual void extractTerms(SetTerm terms);

    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    /// Prints a user-readable version of this query.
    virtual String toString(const String& field);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    friend class ConstantWeight;
    friend class ConstantScorer;
};

}

}
#endif
