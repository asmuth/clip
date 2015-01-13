/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/MultiSearcher.h"
#include "fnord-fts/search/_MultiSearcher.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/search/ReaderUtil.h"
#include "fnord-fts/search/HitQueue.h"
#include "fnord-fts/search/Query.h"
#include "fnord-fts/search/FieldDocSortedHitQueue.h"
#include "fnord-fts/search/TopDocs.h"
#include "fnord-fts/search/ScoreDoc.h"
#include "fnord-fts/search/SortField.h"
#include "fnord-fts/search/TopFieldDocs.h"
#include "fnord-fts/search/FieldDoc.h"
#include "fnord-fts/util/StringUtils.h"
#include "fnord-fts/util/VariantUtils.h"

namespace Lucene {

MultiSearcher::MultiSearcher(Collection<SearchablePtr> searchables) {
    this->searchables = searchables;
    this->_maxDoc = 0;

    this->starts = Collection<int32_t>::newInstance(searchables.size() + 1); // build starts array
    for (int32_t i = 0; i < searchables.size(); ++i) {
        starts[i] = _maxDoc;
        _maxDoc += searchables[i]->maxDoc(); // compute maxDocs
    }
    starts[searchables.size()] = _maxDoc;
}

MultiSearcher::~MultiSearcher() {
}

Collection<SearchablePtr> MultiSearcher::getSearchables() {
    return searchables;
}

Collection<int32_t> MultiSearcher::getStarts() {
    return starts;
}

void MultiSearcher::close() {
    for (Collection<SearchablePtr>::iterator searchable = searchables.begin(); searchable != searchables.end(); ++searchable) {
        (*searchable)->close();
    }
}

int32_t MultiSearcher::docFreq(const TermPtr& term) {
    int32_t docFreq = 0;
    for (Collection<SearchablePtr>::iterator searchable = searchables.begin(); searchable != searchables.end(); ++searchable) {
        docFreq += (*searchable)->docFreq(term);
    }
    return docFreq;
}

DocumentPtr MultiSearcher::doc(int32_t n) {
    int32_t i = subSearcher(n); // find searcher index
    return searchables[i]->doc(n - starts[i]); // dispatch to searcher
}

DocumentPtr MultiSearcher::doc(int32_t n, const FieldSelectorPtr& fieldSelector) {
    int32_t i = subSearcher(n); // find searcher index
    return searchables[i]->doc(n - starts[i], fieldSelector); // dispatch to searcher
}

int32_t MultiSearcher::subSearcher(int32_t n) {
    return ReaderUtil::subIndex(n, starts);
}

int32_t MultiSearcher::subDoc(int32_t n) {
    return n - starts[subSearcher(n)];
}

int32_t MultiSearcher::maxDoc() {
    return _maxDoc;
}

TopDocsPtr MultiSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) {
    HitQueuePtr hq(newLucene<HitQueue>(n, false));
    int32_t totalHits = 0;

    for (int32_t i = 0; i < searchables.size(); ++i) { // search each searcher
        TopDocsPtr docs(newLucene<MultiSearcherCallableNoSort>(SynchronizePtr(), searchables[i], weight, filter, n, hq, i, starts)->call());
        totalHits += docs->totalHits; // update totalHits
    }

    Collection<ScoreDocPtr> scoreDocs(Collection<ScoreDocPtr>::newInstance(hq->size()));
    for (int32_t i = hq->size() - 1; i >= 0; --i) { // put docs in array
        scoreDocs[i] = hq->pop();
    }

    double maxScore = totalHits == 0 ? -std::numeric_limits<double>::infinity() : scoreDocs[0]->score;

    return newLucene<TopDocs>(totalHits, scoreDocs, maxScore);
}

TopFieldDocsPtr MultiSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) {
    FieldDocSortedHitQueuePtr hq(newLucene<FieldDocSortedHitQueue>(n));
    int32_t totalHits = 0;

    double maxScore = -std::numeric_limits<double>::infinity();

    for (int32_t i = 0; i < searchables.size(); ++i) { // search each searcher
        TopFieldDocsPtr docs(newLucene<MultiSearcherCallableWithSort>(SynchronizePtr(), searchables[i], weight, filter, n, hq, sort, i, starts)->call());
        totalHits += docs->totalHits; // update totalHits
        maxScore = std::max(maxScore, docs->maxScore);
    }

    Collection<ScoreDocPtr> scoreDocs(Collection<ScoreDocPtr>::newInstance(hq->size()));
    for (int32_t i = hq->size() - 1; i >= 0; --i) { // put docs in array
        scoreDocs[i] = hq->pop();
    }

    return newLucene<TopFieldDocs>(totalHits, scoreDocs, hq->getFields(), maxScore);
}

void MultiSearcher::search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results) {
    for (int32_t i = 0; i < searchables.size(); ++i) {
        int32_t start = starts[i];
        CollectorPtr hc = newLucene<MultiSearcherCollector>(results, start);
        searchables[i]->search(weight, filter, hc);
    }
}

QueryPtr MultiSearcher::rewrite(const QueryPtr& query) {
    Collection<QueryPtr> queries(Collection<QueryPtr>::newInstance(searchables.size()));
    for (int32_t i = 0; i < searchables.size(); ++i) {
        queries[i] = searchables[i]->rewrite(query);
    }
    return queries[0]->combine(queries);
}

ExplanationPtr MultiSearcher::explain(const WeightPtr& weight, int32_t doc) {
    int32_t i = subSearcher(doc); // find searcher index
    return searchables[i]->explain(weight, doc - starts[i]); // dispatch to searcher
}

WeightPtr MultiSearcher::createWeight(const QueryPtr& query) {
    // step 1
    QueryPtr rewrittenQuery(rewrite(query));

    // step 2
    SetTerm terms(SetTerm::newInstance());
    rewrittenQuery->extractTerms(terms);

    // step3
    Collection<TermPtr> allTermsArray(Collection<TermPtr>::newInstance(terms.begin(), terms.end()));
    Collection<int32_t> aggregatedDfs(Collection<int32_t>::newInstance(terms.size()));
    for (Collection<SearchablePtr>::iterator searchable = searchables.begin(); searchable != searchables.end(); ++searchable) {
        Collection<int32_t> dfs((*searchable)->docFreqs(allTermsArray));
        for (int32_t j = 0; j < aggregatedDfs.size(); ++j) {
            aggregatedDfs[j] += dfs[j];
        }
    }

    MapTermInt dfMap(MapTermInt::newInstance());
    for (int32_t i = 0; i < allTermsArray.size(); ++i) {
        dfMap.put(allTermsArray[i], aggregatedDfs[i]);
    }

    // step4
    int32_t numDocs = maxDoc();
    CachedDfSourcePtr cacheSim(newLucene<CachedDfSource>(dfMap, numDocs, getSimilarity()));

    return rewrittenQuery->weight(cacheSim);
}

CachedDfSource::CachedDfSource(MapTermInt dfMap, int32_t maxDoc, const SimilarityPtr& similarity) {
    this->dfMap = dfMap;
    this->_maxDoc = maxDoc;
    setSimilarity(similarity);
}

CachedDfSource::~CachedDfSource() {
}

int32_t CachedDfSource::docFreq(const TermPtr& term) {
    MapTermInt::iterator df = dfMap.find(term);
    if (df == dfMap.end()) {
        boost::throw_exception(IllegalArgumentException(L"df for term " + term->text() + L" not available"));
    }
    return df->second;
}

Collection<int32_t> CachedDfSource::docFreqs(Collection<TermPtr> terms) {
    Collection<int32_t> result(Collection<int32_t>::newInstance(terms.size()));
    for (int32_t i = 0; i < terms.size(); ++i) {
        result[i] = docFreq(terms[i]);
    }
    return result;
}

int32_t CachedDfSource::maxDoc() {
    return _maxDoc;
}

QueryPtr CachedDfSource::rewrite(const QueryPtr& query) {
    // This is a bit of a hack. We know that a query which creates a Weight based on this Dummy-Searcher is
    // always already rewritten (see preparedWeight()).  Therefore we just return the unmodified query here.
    return query;
}

void CachedDfSource::close() {
    boost::throw_exception(UnsupportedOperationException());
}

DocumentPtr CachedDfSource::doc(int32_t n) {
    boost::throw_exception(UnsupportedOperationException());
    return DocumentPtr();
}

DocumentPtr CachedDfSource::doc(int32_t n, const FieldSelectorPtr& fieldSelector) {
    boost::throw_exception(UnsupportedOperationException());
    return DocumentPtr();
}

ExplanationPtr CachedDfSource::explain(const WeightPtr& weight, int32_t doc) {
    boost::throw_exception(UnsupportedOperationException());
    return ExplanationPtr();
}

void CachedDfSource::search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results) {
    boost::throw_exception(UnsupportedOperationException());
}

TopDocsPtr CachedDfSource::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) {
    boost::throw_exception(UnsupportedOperationException());
    return TopDocsPtr();
}

TopFieldDocsPtr CachedDfSource::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) {
    boost::throw_exception(UnsupportedOperationException());
    return TopFieldDocsPtr();
}

MultiSearcherCallableNoSort::MultiSearcherCallableNoSort(const SynchronizePtr& lock, const SearchablePtr& searchable, const WeightPtr& weight,
        const FilterPtr& filter, int32_t nDocs, const HitQueuePtr& hq, int32_t i,
        Collection<int32_t> starts) {
    this->lock = lock;
    this->searchable = searchable;
    this->weight = weight;
    this->filter = filter;
    this->nDocs = nDocs;
    this->hq = hq;
    this->i = i;
    this->starts = starts;
}

MultiSearcherCallableNoSort::~MultiSearcherCallableNoSort() {
}

TopDocsPtr MultiSearcherCallableNoSort::call() {
    TopDocsPtr docs(searchable->search(weight, filter, nDocs));
    Collection<ScoreDocPtr> scoreDocs(docs->scoreDocs);
    for (int32_t j = 0; j < scoreDocs.size(); ++j) { // merge scoreDocs into hq
        ScoreDocPtr scoreDoc(scoreDocs[j]);
        scoreDoc->doc += starts[i]; // convert doc

        SyncLock syncLock(lock);
        if (scoreDoc == hq->addOverflow(scoreDoc)) {
            break;
        }
    }
    return docs;
}

MultiSearcherCallableWithSort::MultiSearcherCallableWithSort(const SynchronizePtr& lock, const SearchablePtr& searchable, const WeightPtr& weight,
        const FilterPtr& filter, int32_t nDocs, const FieldDocSortedHitQueuePtr& hq,
        const SortPtr& sort, int32_t i, Collection<int32_t> starts) {
    this->lock = lock;
    this->searchable = searchable;
    this->weight = weight;
    this->filter = filter;
    this->nDocs = nDocs;
    this->hq = hq;
    this->i = i;
    this->starts = starts;
    this->sort = sort;
}

MultiSearcherCallableWithSort::~MultiSearcherCallableWithSort() {
}

TopFieldDocsPtr MultiSearcherCallableWithSort::call() {
    TopFieldDocsPtr docs(searchable->search(weight, filter, nDocs, sort));
    // If one of the Sort fields is FIELD_DOC, need to fix its values, so that it will break ties by doc Id
    // properly. Otherwise, it will compare to 'relative' doc Ids, that belong to two different searchables.
    for (int32_t j = 0; j < docs->fields.size(); ++j) {
        if (docs->fields[j]->getType() == SortField::DOC) {
            // iterate over the score docs and change their fields value
            for (int32_t j2 = 0; j2 < docs->scoreDocs.size(); ++j2) {
                FieldDocPtr fd(std::dynamic_pointer_cast<FieldDoc>(docs->scoreDocs[j2]));
                fd->fields[j] = VariantUtils::get<int32_t>(fd->fields[j]) + starts[i];
            }
            break;
        }
    }

    {
        SyncLock syncLock(lock);
        hq->setFields(docs->fields);
    }

    Collection<ScoreDocPtr> scoreDocs(docs->scoreDocs);
    for (int32_t j = 0; j < scoreDocs.size(); ++j) { // merge scoreDocs into hq
        FieldDocPtr fieldDoc(std::dynamic_pointer_cast<FieldDoc>(scoreDocs[j]));
        fieldDoc->doc += starts[i]; // convert doc

        SyncLock syncLock(lock);
        if (fieldDoc == hq->addOverflow(fieldDoc)) {
            break;
        }
    }

    return docs;
}

MultiSearcherCollector::MultiSearcherCollector(const CollectorPtr& collector, int32_t start) {
    this->collector = collector;
    this->start = start;
}

MultiSearcherCollector::~MultiSearcherCollector() {
}

void MultiSearcherCollector::setScorer(const ScorerPtr& scorer) {
    collector->setScorer(scorer);
}

void MultiSearcherCollector::collect(int32_t doc) {
    collector->collect(doc);
}

void MultiSearcherCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    collector->setNextReader(reader, start + docBase);
}

bool MultiSearcherCollector::acceptsDocsOutOfOrder() {
    return collector->acceptsDocsOutOfOrder();
}

}
