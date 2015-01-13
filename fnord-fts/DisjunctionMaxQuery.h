/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DISJUNCTIONMAXQUERY_H
#define DISJUNCTIONMAXQUERY_H

#include "Query.h"

namespace Lucene {

/// A query that generates the union of documents produced by its subqueries, and that scores each
/// document with the maximum score for that document as produced by any subquery, plus a tie breaking
/// increment for any additional matching subqueries.  This is useful when searching for a word in
/// multiple fields with different boost factors (so that the fields cannot be combined equivalently
/// into a single search field).  We want the primary score to be the one associated with the highest
/// boost, not the sum of the field scores (as BooleanQuery would give).  If the query is "albino
/// elephant" this ensures that "albino" matching one field and "elephant" matching another gets a
/// higher score than "albino" matching both fields.  To get this result, use both BooleanQuery and
/// DisjunctionMaxQuery: for each term a DisjunctionMaxQuery searches for it in each field, while the
/// set of these DisjunctionMaxQuery's is combined into a BooleanQuery.  The tie breaker capability
/// allows results that include the same term in multiple fields to be judged better than results that
/// include this term in only the best of those multiple fields, without confusing this with the better
/// case of two different terms in the multiple fields.
class DisjunctionMaxQuery : public Query {
public:
    /// Creates a new empty DisjunctionMaxQuery.  Use add() to add the subqueries.
    /// @param tieBreakerMultiplier the score of each non-maximum disjunct for a document is multiplied
    /// by this weight and added into the final score.  If non-zero, the value should be small, on the
    /// order of 0.1, which says that 10 occurrences of word in a lower-scored field that is also in a
    /// higher scored field is just as good as a unique word in the lower scored field (ie., one that is
    /// not in any higher scored field.
    DisjunctionMaxQuery(double tieBreakerMultiplier = 0.0);

    /// Creates a new DisjunctionMaxQuery
    /// @param disjuncts A Collection<QueryPtr> of all the disjuncts to add
    /// @param tieBreakerMultiplier The weight to give to each matching non-maximum disjunct
    DisjunctionMaxQuery(Collection<QueryPtr> disjuncts, double tieBreakerMultiplier);

    virtual ~DisjunctionMaxQuery();

    LUCENE_CLASS(DisjunctionMaxQuery);

protected:
    /// The subqueries
    Collection<QueryPtr> disjuncts;

    /// Multiple of the non-max disjunct scores added into our final score.  Non-zero values support tie-breaking.
    double tieBreakerMultiplier;

public:
    using Query::toString;

    /// Add a subquery to this disjunction
    /// @param query the disjunct added
    void add(const QueryPtr& query);

    /// Add a collection of disjuncts to this disjunction
    void add(Collection<QueryPtr> disjuncts);

    /// An iterator over the disjuncts
    Collection<QueryPtr>::iterator begin();
    Collection<QueryPtr>::iterator end();

    /// Create the Weight used to score us
    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    /// Optimize our representation and our subqueries representations
    /// @param reader the IndexReader we query
    /// @return an optimized copy of us (which may not be a copy if there is nothing to optimize)
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    /// Create a shallow copy of us - used in rewriting if necessary
    /// @return a copy of us (but reuse, don't copy, our subqueries)
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Adds all terms occurring in this query to the terms set.
    virtual void extractTerms(SetTerm terms);

    /// Pretty print us.
    /// @param field the field to which we are applied
    /// @return a string that shows what we do, of the form "(disjunct1 | disjunct2 | ... | disjunctn)^boost"
    virtual String toString(const String& field);

    /// @return true if other is a DisjunctionMaxQuery with the same boost and the same subqueries, in the
    /// same order, as us
    virtual bool equals(const LuceneObjectPtr& other);

    virtual int32_t hashCode();

    friend class DisjunctionMaxWeight;
};

}

#endif
