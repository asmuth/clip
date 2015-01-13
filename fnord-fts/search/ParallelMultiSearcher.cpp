/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include "fnord-fts/search/ParallelMultiSearcher.h"
#include "fnord-fts/search/_MultiSearcher.h"
#include "fnord-fts/search/HitQueue.h"
#include "fnord-fts/search/FieldDocSortedHitQueue.h"
#include "fnord-fts/search/FieldDoc.h"
#include "fnord-fts/search/TopFieldDocs.h"
#include "fnord-fts/util/ThreadPool.h"

namespace Lucene {

ParallelMultiSearcher::ParallelMultiSearcher(Collection<SearchablePtr> searchables) : MultiSearcher(searchables) {
}

ParallelMultiSearcher::~ParallelMultiSearcher() {
}

int32_t ParallelMultiSearcher::docFreq(const TermPtr& term) {
    ThreadPoolPtr threadPool(ThreadPool::getInstance());
    Collection<FuturePtr> searchThreads(Collection<FuturePtr>::newInstance(searchables.size()));
    for (int32_t i = 0; i < searchables.size(); ++i) {
        searchThreads[i] = threadPool->scheduleTask(boost::protect(boost::bind<int32_t>(boost::mem_fn(&Searchable::docFreq), searchables[i], term)));
    }
    int32_t docFreq = 0;
    for (int32_t i = 0; i < searchThreads.size(); ++i) {
        docFreq += searchThreads[i]->get<int32_t>();
    }
    return docFreq;
}

TopDocsPtr ParallelMultiSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) {
    HitQueuePtr hq(newLucene<HitQueue>(n, false));
    SynchronizePtr lock(newInstance<Synchronize>());
    ThreadPoolPtr threadPool(ThreadPool::getInstance());
    Collection<FuturePtr> searchThreads(Collection<FuturePtr>::newInstance(searchables.size()));
    Collection<MultiSearcherCallableNoSortPtr> multiSearcher(Collection<MultiSearcherCallableNoSortPtr>::newInstance(searchables.size()));
    for (int32_t i = 0; i < searchables.size(); ++i) { // search each searchable
        multiSearcher[i] = newLucene<MultiSearcherCallableNoSort>(lock, searchables[i], weight, filter, n, hq, i, starts);
        searchThreads[i] = threadPool->scheduleTask(boost::protect(boost::bind<TopDocsPtr>(boost::mem_fn(&MultiSearcherCallableNoSort::call), multiSearcher[i])));
    }

    int32_t totalHits = 0;
    double maxScore = -std::numeric_limits<double>::infinity();

    for (int32_t i = 0; i < searchThreads.size(); ++i) {
        TopDocsPtr topDocs(searchThreads[i]->get<TopDocsPtr>());
        totalHits += topDocs->totalHits;
        maxScore = std::max(maxScore, topDocs->maxScore);
    }

    Collection<ScoreDocPtr> scoreDocs(Collection<ScoreDocPtr>::newInstance(hq->size()));
    for (int32_t i = hq->size() - 1; i >= 0; --i) { // put docs in array
        scoreDocs[i] = hq->pop();
    }

    return newLucene<TopDocs>(totalHits, scoreDocs, maxScore);
}

TopFieldDocsPtr ParallelMultiSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) {
    if (!sort) {
        boost::throw_exception(NullPointerException(L"sort must not be null"));
    }
    FieldDocSortedHitQueuePtr hq(newLucene<FieldDocSortedHitQueue>(n));
    SynchronizePtr lock(newInstance<Synchronize>());
    ThreadPoolPtr threadPool(ThreadPool::getInstance());
    Collection<FuturePtr> searchThreads(Collection<FuturePtr>::newInstance(searchables.size()));
    Collection<MultiSearcherCallableWithSortPtr> multiSearcher(Collection<MultiSearcherCallableWithSortPtr>::newInstance(searchables.size()));
    for (int32_t i = 0; i < searchables.size(); ++i) { // search each searchable
        multiSearcher[i] = newLucene<MultiSearcherCallableWithSort>(lock, searchables[i], weight, filter, n, hq, sort, i, starts);
        searchThreads[i] = threadPool->scheduleTask(boost::protect(boost::bind<TopFieldDocsPtr>(boost::mem_fn(&MultiSearcherCallableWithSort::call), multiSearcher[i])));
    }

    int32_t totalHits = 0;
    double maxScore = -std::numeric_limits<double>::infinity();

    for (int32_t i = 0; i < searchThreads.size(); ++i) {
        TopFieldDocsPtr topDocs(searchThreads[i]->get<TopFieldDocsPtr>());
        totalHits += topDocs->totalHits;
        maxScore = std::max(maxScore, topDocs->maxScore);
    }

    Collection<ScoreDocPtr> scoreDocs(Collection<ScoreDocPtr>::newInstance(hq->size()));
    for (int32_t i = hq->size() - 1; i >= 0; --i) { // put docs in array
        scoreDocs[i] = hq->pop();
    }

    return newLucene<TopFieldDocs>(totalHits, scoreDocs, hq->getFields(), maxScore);
}

}
