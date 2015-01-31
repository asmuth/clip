/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PARALLELMULTISEARCHER_H
#define PARALLELMULTISEARCHER_H

#include "fnord-fts/search/MultiSearcher.h"

namespace fnord {
namespace fts {

/// Implements parallel search over a set of Searchables.
///
/// Applications usually need only call the inherited {@link #search(QueryPtr, int32_t)} or
/// {@link #search(QueryPtr, FilterPtr, int32_t)} methods.
class ParallelMultiSearcher : public MultiSearcher {
public:
    /// Creates a {@link Searchable} which searches searchables.
    ParallelMultiSearcher(Collection<SearchablePtr> searchables);
    virtual ~ParallelMultiSearcher();

    LUCENE_CLASS(ParallelMultiSearcher);

public:
    /// Executes each {@link Searchable}'s docFreq() in its own thread and waits for each search to
    /// complete and merge the results back together.
    virtual int32_t docFreq(const TermPtr& term);

    /// A search implementation which executes each {@link Searchable} in its own thread and waits
    /// for each search to complete and merge the results back together.
    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n);

    /// A search implementation allowing sorting which spans a new thread for each Searchable, waits
    /// for each search to complete and merges the results back together.
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort);
};

}

}
#endif
