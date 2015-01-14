/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENESIGNAL_H
#define LUCENESIGNAL_H

#include <boost/thread/condition.hpp>
#include "fnord-fts/fts.h"

namespace fnord {
namespace fts {

/// Utility class to support signaling notifications.
class LuceneSignal {
public:
    LuceneSignal(const SynchronizePtr& objectLock = SynchronizePtr());
    virtual ~LuceneSignal();

protected:
    boost::mutex waitMutex;
    boost::condition signalCondition;
    SynchronizePtr objectLock;

public:
    /// create a new LuceneSignal instance atomically.
    static void createSignal(LuceneSignalPtr& signal, const SynchronizePtr& objectLock);

    /// Wait for signal using an optional timeout.
    void wait(int32_t timeout = 0);

    /// Notify all threads waiting for signal.
    void notifyAll();
};

}

}
#endif
