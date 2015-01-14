/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/PositiveScoresOnlyCollector.h"
#include "fnord-fts/search/ScoreCachingWrappingScorer.h"

namespace Lucene {

PositiveScoresOnlyCollector::PositiveScoresOnlyCollector(const CollectorPtr& collector) {
    this->collector = collector;
}

PositiveScoresOnlyCollector::~PositiveScoresOnlyCollector() {
}

void PositiveScoresOnlyCollector::collect(int32_t doc) {
    if (scorer->score() > 0) {
        collector->collect(doc);
    }
}

void PositiveScoresOnlyCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    collector->setNextReader(reader, docBase);
}

void PositiveScoresOnlyCollector::setScorer(const ScorerPtr& scorer) {
    // Set a ScoreCachingWrappingScorer in case the wrapped Collector will call score() also.
    this->scorer = newLucene<ScoreCachingWrappingScorer>(scorer);
    collector->setScorer(this->scorer);
}

bool PositiveScoresOnlyCollector::acceptsDocsOutOfOrder() {
    return collector->acceptsDocsOutOfOrder();
}

}
