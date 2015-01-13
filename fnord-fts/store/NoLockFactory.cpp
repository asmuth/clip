/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NoLockFactory.h"
#include "_NoLockFactory.h"

namespace Lucene {

NoLockFactory::~NoLockFactory() {
}

NoLockFactoryPtr NoLockFactory::getNoLockFactory() {
    static NoLockFactoryPtr singleton;
    if (!singleton) {
        singleton = newLucene<NoLockFactory>();
        CycleCheck::addStatic(singleton);
    }
    return singleton;
}

NoLockPtr NoLockFactory::getSingletonLock() {
    // Single instance returned whenever makeLock is called.
    static NoLockPtr singletonLock;
    if (!singletonLock) {
        singletonLock = newLucene<NoLock>();
        CycleCheck::addStatic(singletonLock);
    }
    return singletonLock;
}

LockPtr NoLockFactory::makeLock(const String& lockName) {
    return getSingletonLock();
}

void NoLockFactory::clearLock(const String& lockName) {
}

NoLock::~NoLock() {
}

bool NoLock::obtain() {
    return true;
}

void NoLock::release() {
}

bool NoLock::isLocked() {
    return false;
}

String NoLock::toString() {
    return getClassName();
}

}
