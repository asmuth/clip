/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SingleInstanceLockFactory.h"
#include "_SingleInstanceLockFactory.h"

namespace Lucene {

SingleInstanceLockFactory::SingleInstanceLockFactory() {
    locks = HashSet<String>::newInstance();
}

SingleInstanceLockFactory::~SingleInstanceLockFactory() {
}

LockPtr SingleInstanceLockFactory::makeLock(const String& lockName) {
    // We do not use the LockPrefix at all, because the private HashSet instance
    // effectively scopes the locking to this single Directory instance.
    return newLucene<SingleInstanceLock>(locks, lockName);
}

void SingleInstanceLockFactory::clearLock(const String& lockName) {
    SyncLock syncLock(&locks);
    locks.remove(lockName);
}

SingleInstanceLock::SingleInstanceLock(HashSet<String> locks, const String& lockName) {
    this->locks = locks;
    this->lockName = lockName;
}

SingleInstanceLock::~SingleInstanceLock() {
}

bool SingleInstanceLock::obtain() {
    SyncLock syncLock(&locks);
    return locks.add(lockName);
}

void SingleInstanceLock::release() {
    SyncLock syncLock(&locks);
    locks.remove(lockName);
}

bool SingleInstanceLock::isLocked() {
    SyncLock syncLock(&locks);
    return locks.contains(lockName);
}

String SingleInstanceLock::toString() {
    return lockName;
}

}
