/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEARCHABLE_H
#define SEARCHABLE_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// The interface for search implementations.
///
/// Searchable is the abstract network protocol for searching.  Implementations provide search over a single
/// index, over multiple indices, and over indices on remote servers.
///
/// Queries, filters and sort criteria are designed to be compact so that they may be efficiently passed to a
/// remote index, with only the top-scoring hits being returned, rather than every matching hit.
///
/// NOTE: this interface is kept public for convenience.  Since it is not expected to be implemented directly,
/// it may be changed unexpectedly between releases.
class Searchable {
public:
    LUCENE_INTERFACE(Searchable);
    virtual ~Searchable() {}

public:
    /// Lower-level search API.
    ///
    /// {@link Collector#collect(int32_t)} is called for every document.  Collector-based access to remote
    /// indexes is discouraged.
    ///
    /// Applications should only use this if they need all of the matching documents. The high-level search
    /// API ({@link Searcher#search(QueryPtr, int32_t)}) is usually more efficient, as it skips non-high-scoring
    /// hits.
    ///
    /// @param weight To match documents
    /// @param filter If non-null, used to permit documents to be collected.
    /// @param collector To receive hits
    virtual void search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& collector) = 0;

    /// Frees resources associated with this Searcher.  Be careful not to call this method while you are still
    /// using objects that reference this Searchable.
    virtual void close() = 0;

    /// Returns the number of documents containing term.
    /// @see IndexReader#docFreq(TermPtr)
    virtual int32_t docFreq(const TermPtr& term) = 0;

    /// For each term in the terms array, calculates the number of documents containing term. Returns an array
    /// with these document frequencies. Used to minimize number of remote calls.
    virtual Collection<int32_t> docFreqs(Collection<TermPtr> terms) = 0;

    /// Returns one greater than the largest possible document number.
    /// @see IndexReader#maxDoc()
    virtual int32_t maxDoc() = 0;

    /// Low-level search implementation.  Finds the top n hits for query, applying filter if non-null.
    /// Applications should usually call {@link Searcher#search(QueryPtr, int32_t)} or {@link
    /// Searcher#search(QueryPtr, FilterPtr, int32_t)} instead.
    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) = 0;

    /// Returns the stored fields of document i.
    /// @see IndexReader#document(int32_t)
    virtual DocumentPtr doc(int32_t n) = 0;

    /// Get the {@link Document} at the n'th position. The {@link FieldSelector} may be used to determine what
    /// {@link Field}s to load and how they should be loaded.
    ///
    /// NOTE: If the underlying Reader (more specifically, the underlying FieldsReader) is closed before the
    /// lazy {@link Field} is loaded an exception may be thrown.  If you want the value of a lazy {@link Field}
    /// to be available after closing you must explicitly load it or fetch the Document again with a new loader.
    ///
    /// @param n Get the document at the n'th position
    /// @param fieldSelector The {@link FieldSelector} to use to determine what Fields should be loaded on the
    /// Document.  May be null, in which case all Fields will be loaded.
    /// @return The stored fields of the {@link Document} at the n'th position
    ///
    /// @see IndexReader#document(int32_t, FieldSelectorPtr)
    /// @see Fieldable
    /// @see FieldSelector
    /// @see SetBasedFieldSelector
    /// @see LoadFirstFieldSelector
    virtual DocumentPtr doc(int32_t n, const FieldSelectorPtr& fieldSelector) = 0;

    /// Called to re-write queries into primitive queries.
    virtual QueryPtr rewrite(const QueryPtr& query) = 0;

    /// Low-level implementation method.  Returns an Explanation that describes how doc scored against weight.
    ///
    /// This is intended to be used in developing Similarity implementations, and for good performance, should
    /// not be displayed with every hit.  Computing an explanation is as expensive as executing the query over
    /// the entire index.
    ///
    /// Applications should call {@link Searcher#explain(QueryPtr, int32_t)}.
    virtual ExplanationPtr explain(const WeightPtr& weight, int32_t doc) = 0;

    /// Low-level search implementation with arbitrary sorting.  Finds the top n hits for query, applying filter
    /// if non-null, and sorting the hits by the criteria in sort.
    ///
    /// Applications should usually call {@link Searcher#search(QueryPtr, FilterPtr, int32_t, SortPtr)} instead.
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) = 0;
};

}

#endif
