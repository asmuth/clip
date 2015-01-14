/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/TimeLimitingCollector.h"
#include "fnord-fts/search/_TimeLimitingCollector.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

/// Default timer resolution.
const int32_t TimeLimitingCollector::DEFAULT_RESOLUTION = 20;

int64_t TimeLimitingCollector::resolution = TimeLimitingCollector::DEFAULT_RESOLUTION;

TimeLimitingCollector::TimeLimitingCollector(const CollectorPtr& collector, int64_t timeAllowed) {
    this->DEFAULT_GREEDY = false;
    this->greedy = DEFAULT_GREEDY;
    this->collector = collector;
    this->t0 = TIMER_THREAD()->getMilliseconds();
    this->timeout = t0 + timeAllowed;
    this->docBase = 0;
}

TimeLimitingCollector::~TimeLimitingCollector() {
}

TimerThreadPtr TimeLimitingCollector::TIMER_THREAD() {
    static TimerThreadPtr _TIMER_THREAD;
    if (!_TIMER_THREAD) {
        _TIMER_THREAD = newLucene<TimerThread>();
        CycleCheck::addStatic(_TIMER_THREAD);
    }
    if (!_TIMER_THREAD->isAlive()) {
        _TIMER_THREAD->start();    // start single thread instance
    }
    return _TIMER_THREAD;
}

int64_t TimeLimitingCollector::getResolution() {
    return resolution;
}

void TimeLimitingCollector::setResolution(int64_t newResolution) {
    resolution = std::max(newResolution, (int64_t)5); // 5 milliseconds is about the minimum reasonable time for a wait call.
}

void TimeLimitingCollector::stopTimer() {
    if (TIMER_THREAD()->isAlive()) {
        TIMER_THREAD()->stopThread();
        TIMER_THREAD()->join();
    }
}

bool TimeLimitingCollector::isGreedy() {
    return greedy;
}

void TimeLimitingCollector::setGreedy(bool greedy) {
    this->greedy = greedy;
}

void TimeLimitingCollector::collect(int32_t doc) {
    int64_t time = TIMER_THREAD()->getMilliseconds();
    if (timeout < time) {
        if (greedy) {
            collector->collect(doc);
        }
        boost::throw_exception(TimeExceededException(L"Elapsed time:" + StringUtils::toString(timeout - t0) + L" ms. " +
                               L"Exceeded allowed search time:" + StringUtils::toString(time - t0) + L" ms. " +
                               L"Last doc:" + StringUtils::toString(docBase + doc)));
    }
    collector->collect(doc);
}

void TimeLimitingCollector::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    collector->setNextReader(reader, docBase);
    this->docBase = docBase;
}

void TimeLimitingCollector::setScorer(const ScorerPtr& scorer) {
    collector->setScorer(scorer);
}

bool TimeLimitingCollector::acceptsDocsOutOfOrder() {
    return collector->acceptsDocsOutOfOrder();
}

TimerThread::TimerThread() {
    time = 0;
    _stopThread = false;
}

TimerThread::~TimerThread() {
}

void TimerThread::start() {
    _stopThread = false;
    LuceneThread::start();
}

void TimerThread::run() {
    while (!_stopThread) {
        int64_t resolution;
        {
            SyncLock syncLock(this);
            resolution = TimeLimitingCollector::resolution;
            time += resolution;
        }
        LuceneThread::threadSleep(resolution);
    }
}

int64_t TimerThread::getMilliseconds() {
    SyncLock syncLock(this);
    return time;
}

void TimerThread::stopThread() {
    _stopThread = true;
}

}
