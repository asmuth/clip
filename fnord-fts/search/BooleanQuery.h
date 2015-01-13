/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOLEANQUERY_H
#define BOOLEANQUERY_H

#include "fnord-fts/search/Query.h"
#include "BooleanClause.h"
#include "fnord-fts/search/Weight.h"

namespace Lucene {

/// A Query that matches documents matching boolean combinations of other queries, eg. {@link TermQuery}s,
/// {@link PhraseQuery}s or other BooleanQuerys.
class BooleanQuery : public Query {
public:
    /// Constructs an empty boolean query.
    ///
    /// {@link Similarity#coord(int32_t, int32_t)} may be disabled in scoring, as appropriate. For example,
    /// this score factor does not make sense for most automatically generated queries, like {@link WildcardQuery}
    /// and {@link FuzzyQuery}.
    ///
    /// @param disableCoord disables {@link Similarity#coord(int32_t, int32_t)} in scoring.
    BooleanQuery(bool disableCoord = false);
    virtual ~BooleanQuery();

    LUCENE_CLASS(BooleanQuery);

protected:
    static int32_t maxClauseCount;

    Collection<BooleanClausePtr> clauses;
    bool disableCoord;
    int32_t minNrShouldMatch;

public:
    using Query::toString;

    /// Return the maximum number of clauses permitted, 1024 by default.  Attempts to add more than the permitted
    /// number of clauses cause TooManyClauses to be thrown.
    /// @see #setMaxClauseCount(int32_t)
    static int32_t getMaxClauseCount();

    /// Set the maximum number of clauses permitted per BooleanQuery.  Default value is 1024.
    static void setMaxClauseCount(int32_t maxClauseCount);

    /// Returns true if {@link Similarity#coord(int32_t, int32_t)} is disabled in scoring for this query instance.
    /// @see #BooleanQuery(bool)
    bool isCoordDisabled();

    /// Implement coord disabling.
    virtual SimilarityPtr getSimilarity(const SearcherPtr& searcher);

    /// Specifies a minimum number of the optional BooleanClauses which must be satisfied.
    ///
    /// By default no optional clauses are necessary for a match (unless there are no required clauses).  If this
    /// method is used, then the specified number of clauses is required.
    ///
    /// Use of this method is totally independent of specifying that any specific clauses are required (or prohibited).
    /// This number will only be compared against the number of matching optional clauses.
    ///
    /// @param min the number of optional clauses that must match
    void setMinimumNumberShouldMatch(int32_t min);

    /// Gets the minimum number of the optional BooleanClauses which must be satisfied.
    int32_t getMinimumNumberShouldMatch();

    /// Adds a clause to a boolean query.
    /// @see #getMaxClauseCount()
    void add(const QueryPtr& query, BooleanClause::Occur occur);

    /// Adds a clause to a boolean query.
    /// @see #getMaxClauseCount()
    void add(const BooleanClausePtr& clause);

    /// Returns the set of clauses in this query.
    Collection<BooleanClausePtr> getClauses();

    /// Returns an iterator on the clauses in this query.
    Collection<BooleanClausePtr>::iterator begin();
    Collection<BooleanClausePtr>::iterator end();

    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    virtual void extractTerms(SetTerm terms);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    friend class BooleanWeight;
};

}

#endif
