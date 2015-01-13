/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BufferedDeletes.h"
#include "MergeDocIDRemapper.h"

namespace Lucene {

BufferedDeletes::BufferedDeletes(bool doTermSort) {
    // doTermSort not used: always use sorted term map
    terms = MapTermNum::newInstance();
    queries = MapQueryInt::newInstance();
    docIDs = Collection<int32_t>::newInstance();
    numTerms = 0;
    bytesUsed = 0;
}

BufferedDeletes::~BufferedDeletes() {
}

int32_t BufferedDeletes::size() {
    // We use numTerms not terms.size() intentionally, so that deletes by the same term
    // multiple times "count", ie if you ask to flush every 1000 deletes then even dup'd
    // terms are counted towards that 1000
    return numTerms + queries.size() + docIDs.size();
}

void BufferedDeletes::update(const BufferedDeletesPtr& in) {
    numTerms += in->numTerms;
    bytesUsed += in->bytesUsed;
    terms.putAll(in->terms.begin(), in->terms.end());
    queries.putAll(in->queries.begin(), in->queries.end());
    docIDs.addAll(in->docIDs.begin(), in->docIDs.end());
    in->clear();
}

void BufferedDeletes::clear() {
    terms.clear();
    queries.clear();
    docIDs.clear();
    numTerms = 0;
    bytesUsed = 0;
}

void BufferedDeletes::addBytesUsed(int64_t b) {
    bytesUsed += b;
}

bool BufferedDeletes::any() {
    return (!terms.empty() || !docIDs.empty() || !queries.empty());
}

void BufferedDeletes::remap(const MergeDocIDRemapperPtr& mapper, const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergedDocCount) {
    SyncLock syncLock(this);

    MapTermNum newDeleteTerms;

    // Remap delete-by-term
    if (!terms.empty()) {
        newDeleteTerms = MapTermNum::newInstance();
        for (MapTermNum::iterator entry = terms.begin(); entry != terms.end(); ++entry) {
            newDeleteTerms.put(entry->first, newLucene<Num>(mapper->remap(entry->second->getNum())));
        }
    }

    // Remap delete-by-docID
    Collection<int32_t> newDeleteDocIDs;

    if (!docIDs.empty()) {
        newDeleteDocIDs = Collection<int32_t>::newInstance();
        for (Collection<int32_t>::iterator num = docIDs.begin(); num != docIDs.end(); ++num) {
            newDeleteDocIDs.add(mapper->remap(*num));
        }
    }

    // Remap delete-by-query
    MapQueryInt newDeleteQueries;

    if (!queries.empty()) {
        newDeleteQueries = MapQueryInt::newInstance();
        for (MapQueryInt::iterator entry = queries.begin(); entry != queries.end(); ++entry) {
            newDeleteQueries.put(entry->first, mapper->remap(entry->second));
        }
    }

    if (newDeleteTerms) {
        terms = newDeleteTerms;
    }
    if (newDeleteDocIDs) {
        docIDs = newDeleteDocIDs;
    }
    if (newDeleteQueries) {
        queries = newDeleteQueries;
    }
}

Num::Num(int32_t num) {
    this->num = num;
}

int32_t Num::getNum() {
    return num;
}

void Num::setNum(int32_t num) {
    // Only record the new number if it's greater than the current one.  This is important
    // because if multiple threads are replacing the same doc at nearly the same time, it's
    // possible that one thread that got a higher docID is scheduled before the other threads.
    this->num = std::max(this->num, num);
}

}
