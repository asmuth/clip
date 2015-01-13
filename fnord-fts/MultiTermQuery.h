/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTITERMQUERY_H
#define MULTITERMQUERY_H

#include "Query.h"

namespace Lucene {

/// An abstract {@link Query} that matches documents containing a subset of terms provided by a {@link
/// FilteredTermEnum} enumeration.
///
/// This query cannot be used directly; you must subclass it and define {@link #getEnum} to provide a
/// {@link FilteredTermEnum} that iterates through the terms to be matched.
///
/// NOTE: if {@link #setRewriteMethod} is either {@link #CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE} or {@link
/// #SCORING_BOOLEAN_QUERY_REWRITE}, you may encounter a {@link BooleanQuery.TooManyClauses} exception
/// during searching, which happens when the number of terms to be searched exceeds {@link
/// BooleanQuery#getMaxClauseCount()}.  Setting {@link #setRewriteMethod} to {@link
/// #CONSTANT_SCORE_FILTER_REWRITE} prevents this.
///
/// The recommended rewrite method is {@link #CONSTANT_SCORE_AUTO_REWRITE_DEFAULT}: it doesn't spend CPU
/// computing unhelpful scores, and it tries to pick the most performant rewrite method given the query.
///
/// Note that {@link QueryParser} produces MultiTermQueries using {@link #CONSTANT_SCORE_AUTO_REWRITE_DEFAULT}
/// by default.
class MultiTermQuery : public Query {
public:
    MultiTermQuery();
    virtual ~MultiTermQuery();

    LUCENE_CLASS(MultiTermQuery);

protected:
    RewriteMethodPtr rewriteMethod;
    int32_t numberOfTerms;

public:
    /// A rewrite method that first creates a private Filter, by visiting each term in sequence and marking
    /// all docs for that term.  Matching documents are assigned a constant score equal to the query's boost.
    ///
    /// This method is faster than the BooleanQuery rewrite methods when the number of matched terms or matched
    /// documents is non-trivial.  Also, it will never hit an errant TooManyClauses exception.
    ///
    /// @see #setRewriteMethod
    static RewriteMethodPtr CONSTANT_SCORE_FILTER_REWRITE();

    /// A rewrite method that first translates each term into {@link BooleanClause.Occur#SHOULD} clause in a
    /// BooleanQuery, and keeps the scores as computed by the query.  Note that typically such scores are
    /// meaningless to the user, and require non-trivial CPU to compute, so it's almost always better to use
    /// {@link #CONSTANT_SCORE_AUTO_REWRITE_DEFAULT} instead.
    ///
    /// NOTE: This rewrite method will hit {@link BooleanQuery.TooManyClauses} if the number of terms exceeds
    /// {@link BooleanQuery#getMaxClauseCount}.
    ///
    /// @see #setRewriteMethod
    static RewriteMethodPtr SCORING_BOOLEAN_QUERY_REWRITE();

    /// Like {@link #SCORING_BOOLEAN_QUERY_REWRITE} except scores are not computed.  Instead, each matching
    /// document receives a constant score equal to the query's boost.
    ///
    /// NOTE: This rewrite method will hit TooManyClauses if the number of terms exceeds {@link
    /// BooleanQuery#getMaxClauseCount}.
    ///
    /// @see #setRewriteMethod
    static RewriteMethodPtr CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE();

    /// Read-only default instance of {@link ConstantScoreAutoRewrite}, with {@link
    /// ConstantScoreAutoRewrite#setTermCountCutoff} set to {@link ConstantScoreAutoRewrite#DEFAULT_TERM_COUNT_CUTOFF}
    /// and {@link ConstantScoreAutoRewrite#setDocCountPercent} set to {@link
    /// ConstantScoreAutoRewrite#DEFAULT_DOC_COUNT_PERCENT}.  Note that you cannot alter the configuration of
    /// this instance; you'll need to create a private instance instead.
    static RewriteMethodPtr CONSTANT_SCORE_AUTO_REWRITE_DEFAULT();

    /// Return the number of unique terms visited during execution of the query.  If there are many of them,
    /// you may consider using another query type or optimize your total term count in index.
    ///
    /// This method is not thread safe, be sure to only call it when no query is running!  If you re-use the
    /// same query instance for another search, be sure to first reset the term counter with {@link
    /// #clearTotalNumberOfTerms}.
    ///
    /// On optimized indexes / no MultiReaders, you get the correct number of unique terms for the whole index.
    /// Use this number to compare different queries.  For non-optimized indexes this number can also be achieved
    /// in non-constant-score mode.  In constant-score mode you get the total number of terms seeked for all
    /// segments / sub-readers.
    /// @see #clearTotalNumberOfTerms
    int32_t getTotalNumberOfTerms();

    /// Resets the counting of unique terms.  Do this before executing the query/filter.
    /// @see #getTotalNumberOfTerms
    void clearTotalNumberOfTerms();

    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    /// @see #setRewriteMethod
    virtual RewriteMethodPtr getRewriteMethod();

    /// Sets the rewrite method to be used when executing the query.  You can use one of the four core methods,
    /// or implement your own subclass of {@link RewriteMethod}.
    virtual void setRewriteMethod(const RewriteMethodPtr& method);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);

protected:
    /// Construct the enumeration to be used, expanding the pattern term.
    virtual FilteredTermEnumPtr getEnum(const IndexReaderPtr& reader) = 0;

    void incTotalNumberOfTerms(int32_t inc);

    friend class MultiTermQueryWrapperFilter;
    friend class ScoringBooleanQueryRewrite;
    friend class ConstantScoreAutoRewrite;
};

/// Abstract class that defines how the query is rewritten.
class RewriteMethod : public LuceneObject {
public:
    virtual ~RewriteMethod();
    LUCENE_CLASS(RewriteMethod);

public:
    virtual QueryPtr rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query) = 0;
};

/// A rewrite method that tries to pick the best constant-score rewrite method based on term and document
/// counts from the query.  If both the number of terms and documents is small enough, then {@link
/// #CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE} is used.  Otherwise, {@link #CONSTANT_SCORE_FILTER_REWRITE} is
/// used.
class ConstantScoreAutoRewrite : public RewriteMethod {
public:
    ConstantScoreAutoRewrite();
    virtual ~ConstantScoreAutoRewrite();

    LUCENE_CLASS(ConstantScoreAutoRewrite);

public:
    // Defaults derived from rough tests with a 20.0 million doc Wikipedia index.  With more than 350 terms
    // in the query, the filter method is fastest
    static const int32_t DEFAULT_TERM_COUNT_CUTOFF;

    // If the query will hit more than 1 in 1000 of the docs in the index (0.1%), the filter method is fastest
    static const double DEFAULT_DOC_COUNT_PERCENT;

protected:
    int32_t termCountCutoff;
    double docCountPercent;

public:
    /// If the number of terms in this query is equal to or larger than this setting then {@link
    /// #CONSTANT_SCORE_FILTER_REWRITE} is used.
    virtual void setTermCountCutoff(int32_t count);

    /// @see #setTermCountCutoff
    virtual int32_t getTermCountCutoff();

    /// If the number of documents to be visited in the postings exceeds this specified percentage of the
    /// maxDoc() for the index, then {@link #CONSTANT_SCORE_FILTER_REWRITE} is used.
    /// @param percent 0.0 to 100.0
    virtual void setDocCountPercent(double percent);

    /// @see #setDocCountPercent
    virtual double getDocCountPercent();

    virtual QueryPtr rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query);

    virtual int32_t hashCode();
    virtual bool equals(const LuceneObjectPtr& other);
};

}

#endif
