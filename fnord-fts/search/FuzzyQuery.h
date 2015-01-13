/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FUZZYQUERY_H
#define FUZZYQUERY_H

#include "fnord-fts/search/MultiTermQuery.h"

namespace Lucene {

/// Implements the fuzzy search query.  The similarity measurement is based on the Levenshtein (edit
/// distance) algorithm.
///
/// Warning: this query is not very scalable with its default prefix length of 0 - in this case, *every*
/// term will be enumerated and cause an edit score calculation.
class FuzzyQuery : public MultiTermQuery {
public:
    /// Create a new FuzzyQuery that will match terms with a similarity of at least minimumSimilarity
    /// to term.  If a prefixLength > 0 is specified, a common prefix of that length is also required.
    /// @param term The term to search for
    /// @param minimumSimilarity A value between 0 and 1 to set the required similarity between the query
    /// term and the matching terms.  For example, for a minimumSimilarity of 0.5 a term of the same
    /// length as the query term is considered similar to the query term if the edit distance between
    /// both terms is less than length(term) * 0.5
    /// @param prefixLength Length of common (non-fuzzy) prefix
    FuzzyQuery(const TermPtr& term, double minimumSimilarity, int32_t prefixLength);
    FuzzyQuery(const TermPtr& term, double minimumSimilarity);
    FuzzyQuery(const TermPtr& term);

    virtual ~FuzzyQuery();

    LUCENE_CLASS(FuzzyQuery);

protected:
    double minimumSimilarity;
    int32_t prefixLength;
    bool termLongEnough;

    TermPtr term;

public:
    static double defaultMinSimilarity();
    static const int32_t defaultPrefixLength;

public:
    using MultiTermQuery::toString;

    /// Returns the minimum similarity that is required for this query to match.
    /// @return float value between 0.0 and 1.0
    double getMinSimilarity();

    /// Returns the non-fuzzy prefix length. This is the number of characters at the start of a term that
    /// must be identical (not fuzzy) to the query term if the query is to match that term.
    int32_t getPrefixLength();

    /// Returns the pattern term.
    TermPtr getTerm();

    virtual void setRewriteMethod(const RewriteMethodPtr& method);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual String toString(const String& field);
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);

protected:
    void ConstructQuery(const TermPtr& term, double minimumSimilarity, int32_t prefixLength);

    virtual FilteredTermEnumPtr getEnum(const IndexReaderPtr& reader);
};

}

#endif
