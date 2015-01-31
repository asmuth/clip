/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOLEANCLAUSE_H
#define BOOLEANCLAUSE_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// A clause in a BooleanQuery.
class BooleanClause : public LuceneObject {
public:
    /// Specifies how clauses are to occur in matching documents.
    enum Occur {
        /// Use this operator for clauses that must appear in the matching documents.
        MUST,

        /// Use this operator for clauses that should appear in the matching documents.  For a BooleanQuery
        /// with no MUST clauses one or more SHOULD clauses must match a document for the BooleanQuery to match.
        /// @see BooleanQuery#setMinimumNumberShouldMatch
        SHOULD,

        /// Use this operator for clauses that must not appear in the matching documents.  Note that it is not
        /// possible to search for queries that only consist of a MUST_NOT clause.
        MUST_NOT
    };

public:
    BooleanClause(const QueryPtr& query, Occur occur);
    virtual ~BooleanClause();

    LUCENE_CLASS(BooleanClause);

protected:
    /// The query whose matching documents are combined by the boolean query.
    QueryPtr query;
    Occur occur;

public:
    Occur getOccur();
    void setOccur(Occur occur);

    QueryPtr getQuery();
    void setQuery(const QueryPtr& query);

    bool isProhibited();
    bool isRequired();

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual String toString();
};

}

}
#endif
