/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/IndexSearcher.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/search/TopScoreDocCollector.h"
#include "fnord-fts/search/TopFieldDocs.h"
#include "fnord-fts/search/TopFieldCollector.h"
#include "fnord-fts/search/Weight.h"
#include "fnord-fts/search/DocIdSet.h"
#include "fnord-fts/search/Scorer.h"
#include "fnord-fts/search/Filter.h"
#include "fnord-fts/search/Query.h"
#include "fnord-fts/util/ReaderUtil.h"

namespace Lucene {

IndexSearcher::IndexSearcher(const DirectoryPtr& path, bool readOnly) {
    ConstructSearcher(IndexReader::open(path, readOnly), true);
}

IndexSearcher::IndexSearcher(const IndexReaderPtr& reader) {
    ConstructSearcher(reader, false);
}

IndexSearcher::IndexSearcher(const IndexReaderPtr& reader, Collection<IndexReaderPtr> subReaders, Collection<int32_t> docStarts) {
    this->fieldSortDoTrackScores = false;
    this->fieldSortDoMaxScore = false;
    this->reader = reader;
    this->subReaders = subReaders;
    this->docStarts = docStarts;
    closeReader = false;
}

IndexSearcher::~IndexSearcher() {
}

void IndexSearcher::ConstructSearcher(const IndexReaderPtr& reader, bool closeReader) {
    this->fieldSortDoTrackScores = false;
    this->fieldSortDoMaxScore = false;
    this->reader = reader;
    this->closeReader = closeReader;

    Collection<IndexReaderPtr> subReadersList(Collection<IndexReaderPtr>::newInstance());
    gatherSubReaders(subReadersList, reader);
    subReaders = subReadersList;
    docStarts = Collection<int32_t>::newInstance(subReaders.size());
    int32_t maxDoc = 0;
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        docStarts[i] = maxDoc;
        maxDoc += subReaders[i]->maxDoc();
    }
}

void IndexSearcher::gatherSubReaders(Collection<IndexReaderPtr> allSubReaders, const IndexReaderPtr& reader) {
    ReaderUtil::gatherSubReaders(allSubReaders, reader);
}

IndexReaderPtr IndexSearcher::getIndexReader() {
    return reader;
}

void IndexSearcher::close() {
    if (closeReader) {
        reader->close();
    }
}

int32_t IndexSearcher::docFreq(const TermPtr& term) {
    return reader->docFreq(term);
}

DocumentPtr IndexSearcher::doc(int32_t n) {
    return reader->document(n);
}

DocumentPtr IndexSearcher::doc(int32_t n, const FieldSelectorPtr& fieldSelector) {
    return reader->document(n, fieldSelector);
}

int32_t IndexSearcher::maxDoc() {
    return reader->maxDoc();
}

TopDocsPtr IndexSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n) {
    if (n <= 0) {
        boost::throw_exception(IllegalArgumentException(L"n must be > 0"));
    }
    TopScoreDocCollectorPtr collector(TopScoreDocCollector::create(std::min(n, reader->maxDoc()), !weight->scoresDocsOutOfOrder()));
    search(weight, filter, collector);
    return collector->topDocs();
}

TopFieldDocsPtr IndexSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort) {
    return search(weight, filter, n, sort, true);
}

TopFieldDocsPtr IndexSearcher::search(const WeightPtr& weight, const FilterPtr& filter, int32_t n, const SortPtr& sort, bool fillFields) {
    TopFieldCollectorPtr collector(TopFieldCollector::create(sort, std::min(n, reader->maxDoc()), fillFields, fieldSortDoTrackScores, fieldSortDoMaxScore, !weight->scoresDocsOutOfOrder()));
    search(weight, filter, collector);
    return std::dynamic_pointer_cast<TopFieldDocs>(collector->topDocs());
}

void IndexSearcher::search(const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& results) {
    if (!filter) {
        for (int32_t i = 0; i < subReaders.size(); ++i) { // search each subreader
            results->setNextReader(subReaders[i], docStarts[i]);
            ScorerPtr scorer(weight->scorer(subReaders[i], !results->acceptsDocsOutOfOrder(), true));
            if (scorer) {
                scorer->score(results);
            }
        }
    } else {
        for (int32_t i = 0; i < subReaders.size(); ++i) { // search each subreader
            results->setNextReader(subReaders[i], docStarts[i]);
            searchWithFilter(subReaders[i], weight, filter, results);
        }
    }
}

void IndexSearcher::searchWithFilter(const IndexReaderPtr& reader, const WeightPtr& weight, const FilterPtr& filter, const CollectorPtr& collector) {
    BOOST_ASSERT(filter);

    ScorerPtr scorer(weight->scorer(reader, true, false));
    if (!scorer) {
        return;
    }

    int32_t docID = scorer->docID();
    BOOST_ASSERT(docID == -1 || docID == DocIdSetIterator::NO_MORE_DOCS);

    DocIdSetPtr filterDocIdSet(filter->getDocIdSet(reader));
    if (!filterDocIdSet) {
        // this means the filter does not accept any documents.
        return;
    }

    DocIdSetIteratorPtr filterIter(filterDocIdSet->iterator());
    if (!filterIter) {
        // this means the filter does not accept any documents.
        return;
    }

    int32_t filterDoc = filterIter->nextDoc();
    int32_t scorerDoc = scorer->advance(filterDoc);

    collector->setScorer(scorer);
    while (true) {
        if (scorerDoc == filterDoc) {
            // Check if scorer has exhausted, only before collecting.
            if (scorerDoc == DocIdSetIterator::NO_MORE_DOCS) {
                break;
            }
            collector->collect(scorerDoc);
            filterDoc = filterIter->nextDoc();
            scorerDoc = scorer->advance(filterDoc);
        } else if (scorerDoc > filterDoc) {
            filterDoc = filterIter->advance(scorerDoc);
        } else {
            scorerDoc = scorer->advance(filterDoc);
        }
    }
}

QueryPtr IndexSearcher::rewrite(const QueryPtr& original) {
    QueryPtr query(original);
    for (QueryPtr rewrittenQuery(query->rewrite(reader)); rewrittenQuery != query; rewrittenQuery = query->rewrite(reader)) {
        query = rewrittenQuery;
    }
    return query;
}

ExplanationPtr IndexSearcher::explain(const WeightPtr& weight, int32_t doc) {
    int32_t n = ReaderUtil::subIndex(doc, docStarts);
    int32_t deBasedDoc = doc - docStarts[n];
    return weight->explain(subReaders[n], deBasedDoc);
}

void IndexSearcher::setDefaultFieldSortScoring(bool doTrackScores, bool doMaxScore) {
    fieldSortDoTrackScores = doTrackScores;
    fieldSortDoMaxScore = doMaxScore;
}

}
