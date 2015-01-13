/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FSLockFactory.h"

namespace Lucene {

FSLockFactory::FSLockFactory() {
}

FSLockFactory::~FSLockFactory() {
}

void FSLockFactory::setLockDir(const String& lockDir) {
    if (!this->lockDir.empty()) {
        boost::throw_exception(IllegalStateException(L"You can set the lock directory for this factory only once."));
    }
    this->lockDir = lockDir;
}

String FSLockFactory::getLockDir() {
    return lockDir;
}

}
