/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXSEARCHER_H
#define INDEXSEARCHER_H

#include "fnord-fts/search/Searcher.h"

namespace Lucene {

/// Implements search over a single IndexReader.
///
/// Applications usually need only call the inherited {@link #search(QueryPtr, int32_t)} or {@link
/// #search(QueryPtr, FilterPtr, int32_t)} methods.  For performance reasons it is recommended to open only
/// one IndexSearcher and use it for all of your searches.
///
/// NOTE: {@link IndexSearcher} instances are completely thread safe, meaning multiple threads can call any
/// of its methods, concurrently.  If your application requires external synchronization, you should not
/// synchronize on the IndexSearcher instance; use your own (non-Lucene) objects instead.
class IndexSearcher : public Searcher {
public:
    /// Creates a searcher searching the index in the named directory.  You should pass readOnly = true,
    /// since it gives much better concurrent performance, unless you intend to do write operations (delete
    /// documents or change norms) with the underlying IndexReader.
    /// @param path Directory where IndexReader will be opened
    /// @param readOnly If true, the underlying IndexReader will be opened readOnly
    IndexSearcher(const DirectoryPtr& path, bool readOnly = true);

    /// Creates a searcher searching the provided index.
    IndexSearcher(const IndexReaderPtr& reader);

    /// Directly specify the reader, subReaders and their docID starts.
    IndexSearcher(const IndexReaderPtr& reader, Collection<IndexReaderPtr> subReaders, Collection<int32_t> docStarts);

    virtual ~IndexSearcher();

    LUCENE_CLASS(IndexSearcher);

public:
    IndexReaderPtr reader;

protected:
    bool closeReader;

    Collection<IndexReaderPtr> subReaders;
    Collection<int32_t> docStarts;

    bool fieldSortDoTrackScores;
    bool fieldSortDoMaxScore;

public:
    /// Return the {@link IndexReader} this searches.
    IndexReaderPtr getIndexReader();

    /// Note that the underlying IndexReader is not closed, if IndexSearcher was constructed with
    /// IndexSearcher(const IndexReaderPtr& reader).  If the IndexReader was supplied implicitly by specifying a
    /// directory, then the IndexReader gets closed.
    virtual void close();

    virtual int32_t docFreq(const TermPtr& term);
    virtual DocumentPtr doc(int32_t n);
    virtual DocumentPtr doc(int32_t n, const FieldSelectorPtr& fieldSelector);
    virtual int32_t maxDoc();

    using Searcher::search;
    using Searcher::explain;

    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n);
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort);

    /// Just like {@link #search(WeightPtr, FilterPtr, int32_t, SortPtr)}, but you choose whether or not the
    /// fields in the returned {@link FieldDoc} instances should be set by specifying fillFields.
    ///
    /// NOTE: this does not compute scores by default.  If you need scores, create a {@link TopFieldCollector}
    /// instance by calling {@link TopFieldCollector#create} and then pass that to {@link #search(WeightPtr,
    /// FilterPtr, CollectorPtr)}.
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort, bool fillFields);

    virtual void search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results);
    virtual QueryPtr rewrite(const QueryPtr& query);
    virtual ExplanationPtr explain(const WeightPtr& weight, int32_t doc);

    /// By default, no scores are computed when sorting by field (using {@link #search(QueryPtr, FilterPtr,
    /// int32_t, SortPtr)}).  You can change that, per IndexSearcher instance, by calling this method.  Note
    /// that this will incur a CPU cost.
    ///
    /// @param doTrackScores If true, then scores are returned for every matching document in {@link TopFieldDocs}.
    /// @param doMaxScore If true, then the max score for all matching docs is computed.
    virtual void setDefaultFieldSortScoring(bool doTrackScores, bool doMaxScore);

protected:
    void ConstructSearcher(const IndexReaderPtr& reader, bool closeReader);
    void gatherSubReaders(Collection<IndexReaderPtr> allSubReaders, const IndexReaderPtr& reader);
    void searchWithFilter(const IndexReaderPtr& reader, const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& collector);
};

}

#endif
