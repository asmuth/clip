/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/TopScoreDocCollector.h"
#include "fnord-fts/search/_TopScoreDocCollector.h"
#include "fnord-fts/search/HitQueue.h"
#include "fnord-fts/search/ScoreDoc.h"
#include "fnord-fts/search/Scorer.h"
#include "fnord-fts/search/TopDocs.h"
#include "fnord-fts/util/MiscUtils.h"

namespace Lucene {

TopScoreDocCollector::TopScoreDocCollector(int32_t numHits) : TopDocsCollector(newLucene<HitQueue>(numHits, true)) {
    // HitQueue implements getSentinelObject to return a ScoreDoc, so we know that at this point top()
    // is already initialized.
    pqTop = pq->top();
    docBase = 0;
}

TopScoreDocCollector::~TopScoreDocCollector() {
}

TopScoreDocCollectorPtr TopScoreDocCollector::create(int32_t numHits, bool docsScoredInOrder) {
    if (docsScoredInOrder) {
        return newLucene<InOrderTopScoreDocCollector>(numHits);
    } else {
        return newLucene<OutOfOrderTopScoreDocCollector>(numHits);
    }
}

TopDocsPtr TopScoreDocCollector::newTopDocs(Collection<ScoreDocPtr> results, int32_t start) {
    if (!results) {
        return EMPTY_TOPDOCS();
    }

    // We need to compute maxScore in order to set it in TopDocs. If start == 0, it means the largest element
    // is already in results, use its score as maxScore.  Otherwise pop everything else, until the largest
    // element is extracted and use its score as maxScore.
    double maxScore = std::numeric_limits<double>::quiet_NaN();
    if (start == 0) {
        maxScore = results[0]->score;
    } else {
        for (int32_t i = pq->size(); i > 1; --i) {
            pq->pop();
        }
        maxScore = pq->pop()->score;
    }

    return newLucene<TopDocs>(totalHits, results, maxScore);
}

void TopScoreDocCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    this->docBase = docBase;
}

void TopScoreDocCollector::setScorer(const ScorerPtr& scorer) {
    this->_scorer = scorer;
}

InOrderTopScoreDocCollector::InOrderTopScoreDocCollector(int32_t numHits) : TopScoreDocCollector(numHits) {
}

InOrderTopScoreDocCollector::~InOrderTopScoreDocCollector() {
}

void InOrderTopScoreDocCollector::collect(int32_t doc) {
    double score = ScorerPtr(_scorer)->score();

    // This collector cannot handle these scores
    BOOST_ASSERT(score != -std::numeric_limits<double>::infinity());
    BOOST_ASSERT(!MiscUtils::isNaN(score));

    ++totalHits;
    if (score <= pqTop->score) {
        // Since docs are returned in-order (ie., increasing doc Id), a document with equal score to
        // pqTop.score cannot compete since HitQueue favours documents with lower doc Ids.  Therefore
        // reject those docs too.
        return;
    }
    pqTop->doc = doc + docBase;
    pqTop->score = score;
    pqTop = pq->updateTop();
}

bool InOrderTopScoreDocCollector::acceptsDocsOutOfOrder() {
    return false;
}

OutOfOrderTopScoreDocCollector::OutOfOrderTopScoreDocCollector(int32_t numHits) : TopScoreDocCollector(numHits) {
}

OutOfOrderTopScoreDocCollector::~OutOfOrderTopScoreDocCollector() {
}

void OutOfOrderTopScoreDocCollector::collect(int32_t doc) {
    double score = ScorerPtr(_scorer)->score();

    // This collector cannot handle NaN
    BOOST_ASSERT(!MiscUtils::isNaN(score));

    ++totalHits;
    doc += docBase;
    if (score < pqTop->score || (score == pqTop->score && doc > pqTop->doc)) {
        return;
    }
    pqTop->doc = doc;
    pqTop->score = score;
    pqTop = pq->updateTop();
}

bool OutOfOrderTopScoreDocCollector::acceptsDocsOutOfOrder() {
    return true;
}

}
