/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEARCHER_H
#define SEARCHER_H

#include "fnord-fts/search/Searchable.h"

namespace Lucene {

/// An abstract base class for search implementations. Implements the main search methods.
///
/// Note that you can only access hits from a Searcher as long as it is not yet closed, otherwise an IO
/// exception will be thrown.
class Searcher : public Searchable, public LuceneObject {
public:
    Searcher();
    virtual ~Searcher();

    LUCENE_CLASS(Searcher);

protected:
    /// The Similarity implementation used by this searcher.
    SimilarityPtr similarity;

public:
    /// Search implementation with arbitrary sorting.  Finds the top n hits for query, applying filter if
    /// non-null, and sorting the hits by the criteria in sort.
    ///
    /// NOTE: this does not compute scores by default; use {@link IndexSearcher#setDefaultFieldSortScoring}
    /// to enable scoring.
    virtual TopFieldDocsPtr search(const QueryPtr& query, const FilterPtr& filter, int32_t n, const SortPtr& sort);

    /// Lower-level search API.
    ///
    /// {@link Collector#collect(int32_t)} is called for every matching document.
    ///
    /// Applications should only use this if they need all of the matching documents.  The high-level
    /// search API ({@link Searcher#search(QueryPtr, int32_t)}) is usually more efficient, as it skips
    /// non-high-scoring hits.
    ///
    /// Note: The score passed to this method is a raw score.  In other words, the score will not necessarily
    /// be a double whose value is between 0 and 1.
    virtual void search(const QueryPtr& query, const CollectorPtr& results);

    /// Lower-level search API.
    ///
    /// {@link Collector#collect(int32_t)} is called for every matching document.  Collector-based access to
    /// remote indexes is discouraged.
    ///
    /// Applications should only use this if they need all of the matching documents.  The high-level search
    /// API ({@link Searcher#search(QueryPtr, FilterPtr, int32_t)}) is usually more efficient, as it skips
    /// non-high-scoring hits.
    ///
    /// @param query To match documents
    /// @param filter If non-null, used to permit documents to be collected.
    /// @param results To receive hits
    virtual void search(const QueryPtr& query, const FilterPtr& filter, const CollectorPtr& results);

    /// Finds the top n hits for query, applying filter if non-null.
    virtual TopDocsPtr search(const QueryPtr& query, const FilterPtr& filter, int32_t n);

    /// Finds the top n hits for query.
    virtual TopDocsPtr search(const QueryPtr& query, int32_t n);

    /// Returns an Explanation that describes how doc scored against query.
    ///
    /// This is intended to be used in developing Similarity implementations, and for good performance,
    /// should not be displayed with every hit.  Computing an explanation is as expensive as executing the
    /// query over the entire index.
    virtual ExplanationPtr explain(const QueryPtr& query, int32_t doc);

    /// Set the Similarity implementation used by this Searcher.
    virtual void setSimilarity(const SimilarityPtr& similarity);

    /// Return the Similarity implementation used by this Searcher.
    ///
    /// This defaults to the current value of {@link Similarity#getDefault()}.
    virtual SimilarityPtr getSimilarity();

    virtual Collection<int32_t> docFreqs(Collection<TermPtr> terms);

    virtual void search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results) = 0;
    virtual void close() = 0;
    virtual int32_t docFreq(const TermPtr& term) = 0;
    virtual int32_t maxDoc() = 0;
    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) = 0;
    virtual DocumentPtr doc(int32_t n) = 0;
    virtual DocumentPtr doc(int32_t n, const FieldSelectorPtr& fieldSelector) = 0;
    virtual QueryPtr rewrite(const QueryPtr& query) = 0;
    virtual ExplanationPtr explain(const WeightPtr& weight, int32_t doc) = 0;
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) = 0;

protected:
    /// Creates a weight for query.
    /// @return New weight
    virtual WeightPtr createWeight(const QueryPtr& query);
};

}

#endif
