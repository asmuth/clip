/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/TopFieldCollector.h"
#include "fnord-fts/search/_TopFieldCollector.h"
#include "fnord-fts/search/FieldValueHitQueue.h"
#include "fnord-fts/search/FieldComparator.h"
#include "fnord-fts/search/FieldDoc.h"
#include "fnord-fts/search/Scorer.h"
#include "fnord-fts/search/Sort.h"
#include "fnord-fts/search/TopFieldDocs.h"

namespace fnord {
namespace fts {

TopFieldCollector::TopFieldCollector(const HitQueueBasePtr& pq, int32_t numHits, bool fillFields) : TopDocsCollector(pq) {
    this->numHits = numHits;
    this->fillFields = fillFields;
    this->maxScore = std::numeric_limits<double>::quiet_NaN();
    this->queueFull = false;
    this->docBase = 0;
}

TopFieldCollector::~TopFieldCollector() {
}

const Collection<ScoreDocPtr> TopFieldCollector::EMPTY_SCOREDOCS() {
    static Collection<ScoreDocPtr> _EMPTY_SCOREDOCS;
    if (!_EMPTY_SCOREDOCS) {
        _EMPTY_SCOREDOCS = Collection<ScoreDocPtr>::newInstance();
    }
    return _EMPTY_SCOREDOCS;
}

TopFieldCollectorPtr TopFieldCollector::create(const SortPtr& sort, int32_t numHits, bool fillFields, bool trackDocScores, bool trackMaxScore, bool docsScoredInOrder) {
    if (sort->fields.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Sort must contain at least one field"));
    }

    FieldValueHitQueuePtr queue(FieldValueHitQueue::create(sort->fields, numHits));
    if (queue->getComparators().size() == 1) {
        if (docsScoredInOrder) {
            if (trackMaxScore) {
                return newLucene<OneComparatorScoringMaxScoreCollector>(queue, numHits, fillFields);
            } else if (trackDocScores) {
                return newLucene<OneComparatorScoringNoMaxScoreCollector>(queue, numHits, fillFields);
            } else {
                return newLucene<OneComparatorNonScoringCollector>(queue, numHits, fillFields);
            }
        } else {
            if (trackMaxScore) {
                return newLucene<OutOfOrderOneComparatorScoringMaxScoreCollector>(queue, numHits, fillFields);
            } else if (trackDocScores) {
                return newLucene<OutOfOrderOneComparatorScoringNoMaxScoreCollector>(queue, numHits, fillFields);
            } else {
                return newLucene<OutOfOrderOneComparatorNonScoringCollector>(queue, numHits, fillFields);
            }
        }
    }

    // multiple comparators
    if (docsScoredInOrder) {
        if (trackMaxScore) {
            return newLucene<MultiComparatorScoringMaxScoreCollector>(queue, numHits, fillFields);
        } else if (trackDocScores) {
            return newLucene<MultiComparatorScoringNoMaxScoreCollector>(queue, numHits, fillFields);
        } else {
            return newLucene<MultiComparatorNonScoringCollector>(queue, numHits, fillFields);
        }
    } else {
        if (trackMaxScore) {
            return newLucene<OutOfOrderMultiComparatorScoringMaxScoreCollector>(queue, numHits, fillFields);
        } else if (trackDocScores) {
            return newLucene<OutOfOrderMultiComparatorScoringNoMaxScoreCollector>(queue, numHits, fillFields);
        } else {
            return newLucene<OutOfOrderMultiComparatorNonScoringCollector>(queue, numHits, fillFields);
        }
    }
}

void TopFieldCollector::add(int32_t slot, int32_t doc, double score) {
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->add(newLucene<FieldValueHitQueueEntry>(slot, docBase + doc, score)));
    queueFull = (totalHits == numHits);
}

void TopFieldCollector::populateResults(Collection<ScoreDocPtr> results, int32_t howMany) {
    if (fillFields) {
        FieldValueHitQueuePtr queue(std::static_pointer_cast<FieldValueHitQueue>(pq));
        for (int32_t i = howMany - 1; i >= 0; --i) {
            results[i] = queue->fillFields(std::static_pointer_cast<FieldValueHitQueueEntry>(queue->pop()));
        }
    } else {
        for (int32_t i = howMany - 1; i >= 0; --i) {
            FieldValueHitQueueEntryPtr entry(std::static_pointer_cast<FieldValueHitQueueEntry>(pq->pop()));
            results[i] = newLucene<FieldDoc>(entry->doc, entry->score);
        }
    }
}

TopDocsPtr TopFieldCollector::newTopDocs(Collection<ScoreDocPtr> results, int32_t start) {
    if (!results) {
        results = EMPTY_SCOREDOCS();
        // Set maxScore to NaN, in case this is a maxScore tracking collector
        maxScore = std::numeric_limits<double>::quiet_NaN();
    }

    // If this is a maxScoring tracking collector and there were no results
    return newLucene<TopFieldDocs>(totalHits, results, std::static_pointer_cast<FieldValueHitQueue>(pq)->getFields(), maxScore);
}

bool TopFieldCollector::acceptsDocsOutOfOrder() {
    return false;
}

OneComparatorNonScoringCollector::OneComparatorNonScoringCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : TopFieldCollector(queue, numHits, fillFields) {
}

OneComparatorNonScoringCollector::~OneComparatorNonScoringCollector() {
}

void OneComparatorNonScoringCollector::initialize() {
    TopFieldCollector::initialize();
    FieldValueHitQueuePtr queue(std::static_pointer_cast<FieldValueHitQueue>(pq));
    comparator = queue->getComparators()[0];
    reverseMul = queue->getReverseMul()[0];
}

void OneComparatorNonScoringCollector::updateBottom(int32_t doc) {
    // bottom.score is already set to NaN in add().
    bottom->doc = docBase + doc;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void OneComparatorNonScoringCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        if ((reverseMul * comparator->compareBottom(doc)) <= 0) {
            // since docs are visited in doc Id order, if compare is 0, it means this document is largest
            // than anything else in the queue, and therefore not competitive.
            return;
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc);
        comparator->setBottom(bottom->slot);
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, std::numeric_limits<double>::quiet_NaN());
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

void OneComparatorNonScoringCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    this->docBase = docBase;
    comparator->setNextReader(reader, docBase);
}

void OneComparatorNonScoringCollector::setScorer(const ScorerPtr& scorer) {
    comparator->setScorer(scorer);
}

OutOfOrderOneComparatorNonScoringCollector::OutOfOrderOneComparatorNonScoringCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : OneComparatorNonScoringCollector(queue, numHits, fillFields) {
}

OutOfOrderOneComparatorNonScoringCollector::~OutOfOrderOneComparatorNonScoringCollector() {
}

void OutOfOrderOneComparatorNonScoringCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        int32_t cmp = reverseMul * comparator->compareBottom(doc);
        if (cmp < 0 || (cmp == 0 && doc + docBase > bottom->doc)) {
            return;
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc);
        comparator->setBottom(bottom->slot);
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, std::numeric_limits<double>::quiet_NaN());
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

bool OutOfOrderOneComparatorNonScoringCollector::acceptsDocsOutOfOrder() {
    return true;
}

OneComparatorScoringNoMaxScoreCollector::OneComparatorScoringNoMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : OneComparatorNonScoringCollector(queue, numHits, fillFields) {
}

OneComparatorScoringNoMaxScoreCollector::~OneComparatorScoringNoMaxScoreCollector() {
}

void OneComparatorScoringNoMaxScoreCollector::updateBottom(int32_t doc, double score) {
    bottom->doc = docBase + doc;
    bottom->score = score;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void OneComparatorScoringNoMaxScoreCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        if ((reverseMul * comparator->compareBottom(doc)) <= 0) {
            // since docs are visited in doc Id order, if compare is 0, it means this document is largest
            // than anything else in the queue, and therefore not competitive.
            return;
        }

        // Compute the score only if the hit is competitive.
        double score = scorer->score();

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc, score);
        comparator->setBottom(bottom->slot);
    } else {
        // Compute the score only if the hit is competitive.
        double score = scorer->score();

        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, score);
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

void OneComparatorScoringNoMaxScoreCollector::setScorer(const ScorerPtr& scorer) {
    this->scorer = scorer;
    comparator->setScorer(scorer);
}

OutOfOrderOneComparatorScoringNoMaxScoreCollector::OutOfOrderOneComparatorScoringNoMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : OneComparatorScoringNoMaxScoreCollector(queue, numHits, fillFields) {
}

OutOfOrderOneComparatorScoringNoMaxScoreCollector::~OutOfOrderOneComparatorScoringNoMaxScoreCollector() {
}

void OutOfOrderOneComparatorScoringNoMaxScoreCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        int32_t cmp = reverseMul * comparator->compareBottom(doc);
        if (cmp < 0 || (cmp == 0 && doc + docBase > bottom->doc)) {
            return;
        }

        // Compute the score only if the hit is competitive.
        double score = scorer->score();

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc, score);
        comparator->setBottom(bottom->slot);
    } else {
        // Compute the score only if the hit is competitive.
        double score = scorer->score();

        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, score);
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

bool OutOfOrderOneComparatorScoringNoMaxScoreCollector::acceptsDocsOutOfOrder() {
    return true;
}

OneComparatorScoringMaxScoreCollector::OneComparatorScoringMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : OneComparatorNonScoringCollector(queue, numHits, fillFields) {
    // Must set maxScore to NEG_INF, or otherwise std::max always returns NaN.
    this->maxScore = -std::numeric_limits<double>::infinity();
}

OneComparatorScoringMaxScoreCollector::~OneComparatorScoringMaxScoreCollector() {
}

void OneComparatorScoringMaxScoreCollector::updateBottom(int32_t doc, double score) {
    bottom->doc = docBase + doc;
    bottom->score = score;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void OneComparatorScoringMaxScoreCollector::collect(int32_t doc) {
    double score = scorer->score();
    if (score > maxScore) {
        maxScore = score;
    }
    ++totalHits;
    if (queueFull) {
        if ((reverseMul * comparator->compareBottom(doc)) <= 0) {
            // since docs are visited in doc Id order, if compare is 0, it means this document is largest
            // than anything else in the queue, and therefore not competitive.
            return;
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc, score);
        comparator->setBottom(bottom->slot);
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, score);
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

void OneComparatorScoringMaxScoreCollector::setScorer(const ScorerPtr& scorer) {
    this->scorer = scorer;
    OneComparatorNonScoringCollector::setScorer(scorer);
}

OutOfOrderOneComparatorScoringMaxScoreCollector::OutOfOrderOneComparatorScoringMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : OneComparatorScoringMaxScoreCollector(queue, numHits, fillFields) {
}

OutOfOrderOneComparatorScoringMaxScoreCollector::~OutOfOrderOneComparatorScoringMaxScoreCollector() {
}

void OutOfOrderOneComparatorScoringMaxScoreCollector::collect(int32_t doc) {
    double score = scorer->score();
    if (score > maxScore) {
        maxScore = score;
    }
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        int32_t cmp = reverseMul * comparator->compareBottom(doc);
        if (cmp < 0 || (cmp == 0 && doc + docBase > bottom->doc)) {
            return;
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        comparator->copy(bottom->slot, doc);
        updateBottom(doc, score);
        comparator->setBottom(bottom->slot);
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        comparator->copy(slot, doc);
        add(slot, doc, score);
        if (queueFull) {
            comparator->setBottom(bottom->slot);
        }
    }
}

bool OutOfOrderOneComparatorScoringMaxScoreCollector::acceptsDocsOutOfOrder() {
    return true;
}

MultiComparatorNonScoringCollector::MultiComparatorNonScoringCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : TopFieldCollector(queue, numHits, fillFields) {
}

MultiComparatorNonScoringCollector::~MultiComparatorNonScoringCollector() {
}

void MultiComparatorNonScoringCollector::initialize() {
    TopFieldCollector::initialize();
    FieldValueHitQueuePtr queue(std::static_pointer_cast<FieldValueHitQueue>(pq));
    comparators = queue->getComparators();
    reverseMul = queue->getReverseMul();
}

void MultiComparatorNonScoringCollector::updateBottom(int32_t doc) {
    // bottom.score is already set to NaN in add().
    bottom->doc = docBase + doc;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void MultiComparatorNonScoringCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // Here c=0. If we're at the last comparator, this doc is not competitive, since docs are
                // visited in doc Id order, which means this doc cannot compete with any other document
                // in the queue.
                return;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        updateBottom(doc);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }
        add(slot, doc, std::numeric_limits<double>::quiet_NaN());
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

void MultiComparatorNonScoringCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    this->docBase = docBase;
    for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
        (*cmp)->setNextReader(reader, docBase);
    }
}

void MultiComparatorNonScoringCollector::setScorer(const ScorerPtr& scorer) {
    // set the scorer on all comparators
    for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
        (*cmp)->setScorer(scorer);
    }
}

OutOfOrderMultiComparatorNonScoringCollector::OutOfOrderMultiComparatorNonScoringCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : MultiComparatorNonScoringCollector(queue, numHits, fillFields) {
}

OutOfOrderMultiComparatorNonScoringCollector::~OutOfOrderMultiComparatorNonScoringCollector() {
}

void OutOfOrderMultiComparatorNonScoringCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // This is the equals case.
                if (doc + docBase > bottom->doc) {
                    // Definitely not competitive
                    return;
                }
                break;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        updateBottom(doc);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }
        add(slot, doc, std::numeric_limits<double>::quiet_NaN());
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

bool OutOfOrderMultiComparatorNonScoringCollector::acceptsDocsOutOfOrder() {
    return true;
}

MultiComparatorScoringMaxScoreCollector::MultiComparatorScoringMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : MultiComparatorNonScoringCollector(queue, numHits, fillFields) {
    // Must set maxScore to NEG_INF, or otherwise std::max always returns NaN.
    this->maxScore = -std::numeric_limits<double>::infinity();
}

MultiComparatorScoringMaxScoreCollector::~MultiComparatorScoringMaxScoreCollector() {
}

void MultiComparatorScoringMaxScoreCollector::updateBottom(int32_t doc, double score) {
    bottom->doc = docBase + doc;
    bottom->score = score;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void MultiComparatorScoringMaxScoreCollector::collect(int32_t doc) {
    double score = ScorerPtr(_scorer)->score();
    if (score > maxScore) {
        maxScore = score;
    }
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // Here c=0. If we're at the last comparator, this doc is not competitive, since docs are
                // visited in doc Id order, which means this doc cannot compete with any other document
                // in the queue.
                return;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        updateBottom(doc, score);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }
        add(slot, doc, score);
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

void MultiComparatorScoringMaxScoreCollector::setScorer(const ScorerPtr& scorer) {
    this->_scorer = scorer;
    MultiComparatorNonScoringCollector::setScorer(scorer);
}

OutOfOrderMultiComparatorScoringMaxScoreCollector::OutOfOrderMultiComparatorScoringMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : MultiComparatorScoringMaxScoreCollector(queue, numHits, fillFields) {
}

OutOfOrderMultiComparatorScoringMaxScoreCollector::~OutOfOrderMultiComparatorScoringMaxScoreCollector() {
}

void OutOfOrderMultiComparatorScoringMaxScoreCollector::collect(int32_t doc) {
    double score = ScorerPtr(_scorer)->score();
    if (score > maxScore) {
        maxScore = score;
    }
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // This is the equals case.
                if (doc + docBase > bottom->doc) {
                    // Definitely not competitive
                    return;
                }
                break;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        updateBottom(doc, score);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }
        add(slot, doc, score);
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

bool OutOfOrderMultiComparatorScoringMaxScoreCollector::acceptsDocsOutOfOrder() {
    return true;
}

MultiComparatorScoringNoMaxScoreCollector::MultiComparatorScoringNoMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : MultiComparatorNonScoringCollector(queue, numHits, fillFields) {
}

MultiComparatorScoringNoMaxScoreCollector::~MultiComparatorScoringNoMaxScoreCollector() {
}

void MultiComparatorScoringNoMaxScoreCollector::updateBottom(int32_t doc, double score) {
    bottom->doc = docBase + doc;
    bottom->score = score;
    bottom = std::static_pointer_cast<FieldValueHitQueueEntry>(pq->updateTop());
}

void MultiComparatorScoringNoMaxScoreCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // Here c=0. If we're at the last comparator, this doc is not competitive, since docs are
                // visited in doc Id order, which means this doc cannot compete with any other document
                // in the queue.
                return;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        // Compute score only if it is competitive.
        double score = ScorerPtr(_scorer)->score();
        updateBottom(doc, score);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }

        // Compute score only if it is competitive.
        double score = ScorerPtr(_scorer)->score();
        add(slot, doc, score);
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

void MultiComparatorScoringNoMaxScoreCollector::setScorer(const ScorerPtr& scorer) {
    this->_scorer = scorer;
    MultiComparatorNonScoringCollector::setScorer(scorer);
}

OutOfOrderMultiComparatorScoringNoMaxScoreCollector::OutOfOrderMultiComparatorScoringNoMaxScoreCollector(const FieldValueHitQueuePtr& queue, int32_t numHits, bool fillFields) : MultiComparatorScoringNoMaxScoreCollector(queue, numHits, fillFields) {
}

OutOfOrderMultiComparatorScoringNoMaxScoreCollector::~OutOfOrderMultiComparatorScoringNoMaxScoreCollector() {
}

void OutOfOrderMultiComparatorScoringNoMaxScoreCollector::collect(int32_t doc) {
    ++totalHits;
    if (queueFull) {
        // Fastmatch: return if this hit is not competitive
        for (int32_t i = 0; ; ++i) {
            int32_t c = reverseMul[i] * comparators[i]->compareBottom(doc);
            if (c < 0) {
                // Definitely not competitive.
                return;
            } else if (c > 0) {
                // Definitely competitive.
                break;
            } else if (i == comparators.size() - 1) {
                // This is the equals case.
                if (doc + docBase > bottom->doc) {
                    // Definitely not competitive
                    return;
                }
                break;
            }
        }

        // This hit is competitive - replace bottom element in queue and adjustTop
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(bottom->slot, doc);
        }

        // Compute score only if it is competitive.
        double score = ScorerPtr(_scorer)->score();
        updateBottom(doc, score);

        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->setBottom(bottom->slot);
        }
    } else {
        // Startup transient: queue hasn't gathered numHits yet
        int32_t slot = totalHits - 1;
        // Copy hit into queue
        for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
            (*cmp)->copy(slot, doc);
        }

        // Compute score only if it is competitive.
        double score = ScorerPtr(_scorer)->score();
        add(slot, doc, score);
        if (queueFull) {
            for (Collection<FieldComparatorPtr>::iterator cmp = comparators.begin(); cmp != comparators.end(); ++cmp) {
                (*cmp)->setBottom(bottom->slot);
            }
        }
    }
}

void OutOfOrderMultiComparatorScoringNoMaxScoreCollector::setScorer(const ScorerPtr& scorer) {
    this->_scorer = scorer;
    MultiComparatorScoringNoMaxScoreCollector::setScorer(scorer);
}

bool OutOfOrderMultiComparatorScoringNoMaxScoreCollector::acceptsDocsOutOfOrder() {
    return true;
}

}

}
