/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/util/LuceneSync.h"
#include "fnord-fts/util/Synchronize.h"
#include "LuceneSignal.h"

namespace Lucene {

LuceneSync::~LuceneSync() {
}

SynchronizePtr LuceneSync::getSync() {
    Synchronize::createSync(objectLock);
    return objectLock;
}

LuceneSignalPtr LuceneSync::getSignal() {
    LuceneSignal::createSignal(objectSignal, getSync());
    return objectSignal;
}

void LuceneSync::lock(int32_t timeout) {
    getSync()->lock();
}

void LuceneSync::unlock() {
    getSync()->unlock();
}

bool LuceneSync::holdsLock() {
    return getSync()->holdsLock();
}

void LuceneSync::wait(int32_t timeout) {
    getSignal()->wait(timeout);
}

void LuceneSync::notifyAll() {
    getSignal()->notifyAll();
}

}
