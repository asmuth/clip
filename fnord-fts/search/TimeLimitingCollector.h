/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TIMELIMITINGCOLLECTOR_H
#define TIMELIMITINGCOLLECTOR_H

#include "fnord-fts/search/Collector.h"

namespace fnord {
namespace fts {

/// The {@link TimeLimitingCollector} is used to timeout search requests that take longer than the maximum
/// allowed search time limit.  After this time is exceeded, the search thread is stopped by throwing a
/// {@link TimeExceededException}.
class TimeLimitingCollector : public Collector {
public:
    /// Create a TimeLimitedCollector wrapper over another {@link Collector} with a specified timeout.
    /// @param collector the wrapped {@link Collector}
    /// @param timeAllowed max time allowed for collecting hits after which TimeExceeded exception is thrown
    TimeLimitingCollector(const CollectorPtr& collector, int64_t timeAllowed);

    virtual ~TimeLimitingCollector();

    LUCENE_CLASS(TimeLimitingCollector);

public:
    /// Default timer resolution.
    /// @see #setResolution(int64_t)
    static const int32_t DEFAULT_RESOLUTION;

    /// Default for {@link #isGreedy()}.
    /// @see #isGreedy()
    bool DEFAULT_GREEDY;

protected:
    static int64_t resolution;
    bool greedy;

    int64_t t0;
    int64_t timeout;
    CollectorPtr collector;

    int32_t docBase;

public:
    /// Return the timer resolution.
    /// @see #setResolution(int64_t)
    static int64_t getResolution();

    /// Set the timer resolution.
    /// The default timer resolution is 20 milliseconds.
    /// This means that a search required to take no longer than 800 milliseconds may be stopped after
    /// 780 to 820 milliseconds.  Note that:
    /// <ul>
    /// <li>Finer (smaller) resolution is more accurate but less efficient.
    /// <li>Setting resolution to less than 5 milliseconds will be silently modified to 5 milliseconds.
    /// <li>Setting resolution smaller than current resolution might take effect only after current resolution.
    /// (Assume current resolution of 20 milliseconds is modified to 5 milliseconds, then it can take up to 20
    /// milliseconds for the change to have effect.
    /// </ul>
    static void setResolution(int64_t newResolution);

    /// Stop timer thread.
    static void stopTimer();

    /// Checks if this time limited collector is greedy in collecting the last hit.  A non greedy collector,
    /// upon a timeout, would throw a TimeExceeded without allowing the wrapped collector to collect current
    /// doc.  A greedy one would first allow the wrapped hit collector to collect current doc and only then
    /// throw a TimeExceeded exception.
    /// @see #setGreedy(boolean)
    bool isGreedy();

    /// Sets whether this time limited collector is greedy.
    /// @param greedy true to make this time limited greedy
    /// @see #isGreedy()
    void setGreedy(bool greedy);

    /// Calls {@link Collector#collect(int)} on the decorated {@link Collector} unless the allowed time has
    /// passed, in which case it throws an exception.
    virtual void collect(int32_t doc);

    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setScorer(const ScorerPtr& scorer);
    virtual bool acceptsDocsOutOfOrder();

protected:
    /// Initialize a single static timer thread to be used by all TimeLimitedCollector instances.
    static TimerThreadPtr TIMER_THREAD();

    friend class TimerThread;
};

}

}
#endif
