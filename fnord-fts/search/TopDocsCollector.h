/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOPDOCSCOLLECTOR_H
#define TOPDOCSCOLLECTOR_H

#include "Collector.h"
#include "fnord-fts/util/PriorityQueue.h"

namespace Lucene {

/// A base class for all collectors that return a {@link TopDocs} output. This collector allows easy extension
/// by providing a single constructor which accepts a {@link PriorityQueue} as well as protected members for
/// that priority queue and a counter of the number of total hits.
///
/// Extending classes can override {@link #topDocs(int32_t, int32_t)} and {@link #getTotalHits()} in order to
/// provide their own implementation.
class TopDocsCollector : public Collector {
public:
    TopDocsCollector(const HitQueueBasePtr& pq);
    virtual ~TopDocsCollector();

    LUCENE_CLASS(TopDocsCollector);

protected:
    /// The priority queue which holds the top documents.  Note that different implementations of PriorityQueue
    /// give different meaning to 'top documents'.  HitQueue for example aggregates the top scoring documents,
    /// while other PQ implementations may hold documents sorted by other criteria.
    HitQueueBasePtr pq;

    /// The total number of documents that the collector encountered.
    int32_t totalHits;

public:
    /// The total number of documents that matched this query.
    virtual int32_t getTotalHits();

    /// Returns the top docs that were collected by this collector.
    virtual TopDocsPtr topDocs();

    /// Returns the documents in the range [start .. pq.size()) that were collected by this collector.  Note that
    /// if start >= pq.size(), an empty TopDocs is returned.
    ///
    /// This method is convenient to call if the application always asks for the last results, starting from the
    /// last 'page'.
    ///
    /// NOTE: you cannot call this method more than once for each search execution. If you need to call it more
    /// than once, passing each time a different start, you should call {@link #topDocs()} and work with the
    /// returned {@link TopDocs} object, which will contain all the results this search execution collected.
    virtual TopDocsPtr topDocs(int32_t start);

    /// Returns the documents in the rage [start .. start + howMany) that were collected by this collector. Note
    /// that if start >= pq.size(), an empty TopDocs is returned, and if pq.size() - start < howMany, then only
    /// the available documents in [start .. pq.size()) are returned.
    ///
    /// This method is useful to call in case pagination of search results is allowed by the search application,
    /// as well as it attempts to optimize the memory used by allocating only as much as requested by howMany.
    ///
    /// NOTE: you cannot call this method more than once for each search execution. If you need to call it more
    /// than once, passing each time a different range, you should call {@link #topDocs()} and work with the
    /// returned {@link TopDocs} object, which will contain all the results this search execution collected.
    virtual TopDocsPtr topDocs(int32_t start, int32_t howMany);

protected:
    /// This is used in case topDocs() is called with illegal parameters, or there simply aren't (enough) results.
    static TopDocsPtr EMPTY_TOPDOCS();

    /// Populates the results array with the ScoreDoc instances.  This can be overridden in case a different
    /// ScoreDoc type should be returned.
    virtual void populateResults(Collection<ScoreDocPtr> results, int32_t howMany);

    /// Returns a {@link TopDocs} instance containing the given results.  If results is null it means there are
    /// no results to return, either because there were 0 calls to collect() or because the arguments to topDocs
    /// were invalid.
    virtual TopDocsPtr newTopDocs(Collection<ScoreDocPtr> results, int32_t start);
};

}

#endif
