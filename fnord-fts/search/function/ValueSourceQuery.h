/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef VALUESOURCEQUERY_H
#define VALUESOURCEQUERY_H

#include "fnord-fts/search/Query.h"

namespace Lucene {

/// A Query that sets the scores of document to the values obtained from a {@link ValueSource}.
///
/// This query provides a score for each and every undeleted document in the index.
///
/// The value source can be based on a (cached) value of an indexed field, but it can also be based on an
/// external source, eg. values read from an external database.
///
/// Score is set as: Score(doc,query) = (query.getBoost() * query.getBoost()) * valueSource(doc).
class ValueSourceQuery : public Query {
public:
    /// Create a value source query
    /// @param valSrc provides the values defines the function to be used for scoring
    ValueSourceQuery(const ValueSourcePtr& valSrc);

    virtual ~ValueSourceQuery();

    LUCENE_CLASS(ValueSourceQuery);

public:
    ValueSourcePtr valSrc;

public:
    using Query::toString;

    virtual QueryPtr rewrite(const IndexReaderPtr& reader);
    virtual void extractTerms(SetTerm terms);
    virtual WeightPtr createWeight(const SearcherPtr& searcher);
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
