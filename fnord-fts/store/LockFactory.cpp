/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LockFactory.h"

namespace Lucene {

LockFactory::~LockFactory() {
}

void LockFactory::setLockPrefix(const String& lockPrefix) {
    this->lockPrefix = lockPrefix;
}

String LockFactory::getLockPrefix() {
    return lockPrefix;
}

}
