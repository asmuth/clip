/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MULTISEARCHER_H
#define _MULTISEARCHER_H

#include "Searcher.h"
#include "Collector.h"

namespace Lucene {

/// Document Frequency cache acting as a Dummy-Searcher.  This class is not a full-fledged Searcher, but
/// only supports the methods necessary to initialize Weights.
class CachedDfSource : public Searcher {
public:
    CachedDfSource(MapTermInt dfMap, int32_t maxDoc, const SimilarityPtr& similarity);
    virtual ~CachedDfSource();

    LUCENE_CLASS(CachedDfSource);

protected:
    MapTermInt dfMap; // Map from Terms to corresponding doc freqs
    int32_t _maxDoc; // document count

public:
    virtual int32_t docFreq(const TermPtr& term);
    virtual Collection<int32_t> docFreqs(Collection<TermPtr> terms);
    virtual int32_t maxDoc();
    virtual QueryPtr rewrite(const QueryPtr& query);
    virtual void close();
    virtual DocumentPtr doc(int32_t n);
    virtual DocumentPtr doc(int32_t n, const FieldSelectorPtr& fieldSelector);
    virtual ExplanationPtr explain(const WeightPtr& weight, int32_t doc);
    virtual void search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results);
    virtual TopDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n);
    virtual TopFieldDocsPtr search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort);
};

/// A subclass for searching a single searchable
class MultiSearcherCallableNoSort : public LuceneObject {
public:
    MultiSearcherCallableNoSort(const SynchronizePtr& lock, const SearchablePtr& searchable, const WeightPtr& weight, const FilterPtr& filter, int32_t nDocs,
                                const HitQueuePtr& hq, int32_t i, Collection<int32_t> starts);
    virtual ~MultiSearcherCallableNoSort();

    LUCENE_CLASS(MultiSearcherCallableNoSort);

protected:
    SynchronizePtr lock;
    SearchablePtr searchable;
    WeightPtr weight;
    FilterPtr filter;
    int32_t nDocs;
    int32_t i;
    HitQueuePtr hq;
    Collection<int32_t> starts;

public:
    TopDocsPtr call();
};

/// A subclass for searching a single searchable
class MultiSearcherCallableWithSort : public LuceneObject {
public:
    MultiSearcherCallableWithSort(const SynchronizePtr& lock, const SearchablePtr& searchable, const WeightPtr& weight, const FilterPtr& filter,
                                  int32_t nDocs, const FieldDocSortedHitQueuePtr& hq, const SortPtr& sort, int32_t i, Collection<int32_t> starts);
    virtual ~MultiSearcherCallableWithSort();

    LUCENE_CLASS(MultiSearcherCallableWithSort);

protected:
    SynchronizePtr lock;
    SearchablePtr searchable;
    WeightPtr weight;
    FilterPtr filter;
    int32_t nDocs;
    int32_t i;
    FieldDocSortedHitQueuePtr hq;
    Collection<int32_t> starts;
    SortPtr sort;

public:
    TopFieldDocsPtr call();
};

class MultiSearcherCollector : public Collector {
public:
    MultiSearcherCollector(const CollectorPtr& collector, int32_t start);
    virtual ~MultiSearcherCollector();

    LUCENE_CLASS(MultiSearcherCollector);

protected:
    CollectorPtr collector;
    int32_t start;

public:
    virtual void setScorer(const ScorerPtr& scorer);
    virtual void collect(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual bool acceptsDocsOutOfOrder();
};

}

#endif
