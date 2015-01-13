/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTIPHRASEQUERY_H
#define MULTIPHRASEQUERY_H

#include "Query.h"

namespace Lucene {

/// MultiPhraseQuery is a generalized version of PhraseQuery, with an added method {@link #add(Term[])}.
/// To use this class, to search for the phrase "Microsoft app*" first use add(Term) on the term "Microsoft",
/// then find all terms that have "app" as prefix using IndexReader.terms(Term), and use
/// MultiPhraseQuery.add(Term[] terms) to add them to the query.
class MultiPhraseQuery : public Query {
public:
    MultiPhraseQuery();
    virtual ~MultiPhraseQuery();

    LUCENE_CLASS(MultiPhraseQuery);

protected:
    String field;
    Collection< Collection<TermPtr> > termArrays;
    Collection<int32_t> positions;
    int32_t slop;

public:
    using Query::toString;

    /// Sets the phrase slop for this query.
    /// @see PhraseQuery#setSlop(int32_t)
    void setSlop(int32_t s);

    /// Gets the phrase slop for this query.
    /// @see PhraseQuery#getSlop()
    int32_t getSlop();

    /// Add a single term at the next position in the phrase.
    /// @see PhraseQuery#add(Term)
    void add(const TermPtr& term);

    /// Add multiple terms at the next position in the phrase.  Any of the terms may match.
    /// @see PhraseQuery#add(Term)
    void add(Collection<TermPtr> terms);

    /// Allows to specify the relative position of terms within the phrase.
    /// @see PhraseQuery#add(Term, int)
    void add(Collection<TermPtr> terms, int32_t position);

    /// Returns a List of the terms in the multiphrase. Do not modify the List or its contents.
    Collection< Collection<TermPtr> > getTermArrays();

    /// Returns the relative positions of terms in this phrase.
    Collection<int32_t> getPositions();

    virtual void extractTerms(SetTerm terms);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);
    virtual WeightPtr createWeight(const SearcherPtr& searcher);

    /// Prints a user-readable version of this query.
    virtual String toString(const String& field);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

protected:
    int32_t termArraysHashCode();
    bool termArraysEquals(Collection< Collection<TermPtr> > first, Collection< Collection<TermPtr> > second);

    friend class MultiPhraseWeight;
};

}

#endif
