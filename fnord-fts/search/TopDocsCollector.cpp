/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/TopDocsCollector.h"
#include "fnord-fts/TopDocs.h"
#include "fnord-fts/HitQueueBase.h"

namespace Lucene {

TopDocsCollector::TopDocsCollector(const HitQueueBasePtr& pq) {
    this->pq = pq;
    this->totalHits = 0;
}

TopDocsCollector::~TopDocsCollector() {
}

TopDocsPtr TopDocsCollector::EMPTY_TOPDOCS() {
    static TopDocsPtr _EMPTY_TOPDOCS;
    if (!_EMPTY_TOPDOCS) {
        _EMPTY_TOPDOCS = newLucene<TopDocs>(0, Collection<ScoreDocPtr>::newInstance(), std::numeric_limits<double>::quiet_NaN());
        CycleCheck::addStatic(_EMPTY_TOPDOCS);
    }
    return _EMPTY_TOPDOCS;
}

void TopDocsCollector::populateResults(Collection<ScoreDocPtr> results, int32_t howMany) {
    for (int32_t i = howMany - 1; i >= 0; --i) {
        results[i] = pq->pop();
    }
}

TopDocsPtr TopDocsCollector::newTopDocs(Collection<ScoreDocPtr> results, int32_t start) {
    return results ? newLucene<TopDocs>(totalHits, results) : EMPTY_TOPDOCS();
}

int32_t TopDocsCollector::getTotalHits() {
    return totalHits;
}

TopDocsPtr TopDocsCollector::topDocs() {
    // In case pq was populated with sentinel values, there might be less results than pq.size().
    // Therefore return all results until either pq.size() or totalHits.
    return topDocs(0, totalHits < pq->size() ? totalHits : pq->size());
}

TopDocsPtr TopDocsCollector::topDocs(int32_t start) {
    // In case pq was populated with sentinel values, there might be less results than pq.size().
    // Therefore return all results until either pq.size() or totalHits.
    return topDocs(start, totalHits < pq->size() ? totalHits : pq->size());
}

TopDocsPtr TopDocsCollector::topDocs(int32_t start, int32_t howMany) {
    // In case pq was populated with sentinel values, there might be less results than pq.size().
    // Therefore return all results until either pq.size() or totalHits.
    int32_t size = totalHits < pq->size() ? totalHits : pq->size();

    // Don't bother to throw an exception, just return an empty TopDocs in case the parameters are
    // invalid or out of range.
    if (start < 0 || start >= size || howMany <= 0) {
        return newTopDocs(Collection<ScoreDocPtr>(), start);
    }

    // We know that start < pq.size, so just fix howMany.
    howMany = std::min(size - start, howMany);
    Collection<ScoreDocPtr> results = Collection<ScoreDocPtr>::newInstance(howMany);

    // pq's pop() returns the 'least' element in the queue, therefore need to discard the first ones,
    // until we reach the requested range. Note that this loop will usually not be executed, since the
    // common usage should be that the caller asks for the last howMany results. However it's needed
    // here for completeness.
    for (int32_t i = pq->size() - start - howMany; i > 0; --i) {
        pq->pop();
    }

    // Get the requested results from pq.
    populateResults(results, howMany);

    return newTopDocs(results, start);
}

}
