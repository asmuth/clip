/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PREFIXQUERY_H
#define PREFIXQUERY_H

#include "MultiTermQuery.h"

namespace Lucene {

/// A Query that matches documents containing terms with a specified prefix. A PrefixQuery is built by
/// QueryParser for input like app*.
///
/// This query uses the {@link MultiTermQuery#CONSTANT_SCORE_AUTO_REWRITE_DEFAULT} rewrite method.
class PrefixQuery : public MultiTermQuery {
public:
    /// Constructs a query for terms starting with prefix.
    PrefixQuery(const TermPtr& prefix);

    virtual ~PrefixQuery();

    LUCENE_CLASS(PrefixQuery);

protected:
    TermPtr prefix;

public:
    using MultiTermQuery::toString;

    /// Returns the prefix of this query.
    TermPtr getPrefix();

    /// Prints a user-readable version of this query.
    virtual String toString(const String& field);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);

protected:
    virtual FilteredTermEnumPtr getEnum(const IndexReaderPtr& reader);
};

}

#endif
