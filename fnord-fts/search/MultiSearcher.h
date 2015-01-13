/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTISEARCHER_H
#define MULTISEARCHER_H

#include "fnord-fts/search/Searcher.h"
#include "fnord-fts/search/Collector.h"

namespace Lucene {

/// Implements search over a set of Searchables.
///
/// Applications usually need only call the inherited {@link #search(QueryPtr, int32_t)} or {@link
/// #search(QueryPtr, FilterPtr, int32_t)} methods.
class MultiSearcher : public Searcher {
public:
    /// Creates a searcher which searches searchers.
    MultiSearcher(Collection<SearchablePtr> searchables);

    virtual ~MultiSearcher();

    LUCENE_CLASS(MultiSearcher);

protected:
    Collection<SearchablePtr> searchables;
    Collection<int32_t> starts;
    int32_t _maxDoc;

public:
    using Searcher::search;

    /// Return the array of {@link Searchable}s this searches.
    Collection<SearchablePtr> getSearchables();

    virtual void close();
    virtual int32_t docFreq(const TermPtr& term);
    virtual DocumentPtr doc(int32_t n);
    virtual DocumentPtr doc(int32_t n, const FieldSelectorPtr& fieldSelector);

    /// Returns index of the searcher for document n in the array used to construct this searcher.
    int32_t subSearcher(int32_t n);

    /// Returns the document number of document n within its sub-index.
    int32_t subDoc(int32_t n);

    virtual int32_t maxDoc();
    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n);
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort);
    virtual void search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results);
    virtual QueryPtr rewrite(const QueryPtr& query);
    virtual ExplanationPtr explain(const WeightPtr& weight, int32_t doc);

protected:
    Collection<int32_t> getStarts();

    /// Create weight in multiple index scenario.
    ///
    /// Distributed query processing is done in the following steps:
    /// 1. rewrite query.
    /// 2. extract necessary terms.
    /// 3. collect dfs for these terms from the Searchables.
    /// 4. create query weight using aggregate dfs.
    /// 5. distribute that weight to Searchables.
    /// 6. merge results.
    ///
    /// Steps 1-4 are done here, 5+6 in the search() methods
    ///
    /// @return rewritten queries
    virtual WeightPtr createWeight(const QueryPtr& query);
};

}

#endif
