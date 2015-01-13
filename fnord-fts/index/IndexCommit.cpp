/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "IndexCommit.h"
#include "Directory.h"

namespace Lucene {

IndexCommit::~IndexCommit() {
}

bool IndexCommit::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    IndexCommitPtr otherCommit(boost::dynamic_pointer_cast<IndexCommit>(other));
    if (!otherCommit) {
        return false;
    }
    return (otherCommit->getDirectory()->equals(getDirectory()) && otherCommit->getVersion() == getVersion());
}

int32_t IndexCommit::hashCode() {
    return (getDirectory()->hashCode() + (int32_t)getVersion());
}

int64_t IndexCommit::getTimestamp() {
    return getDirectory()->fileModified(getSegmentsFileName());
}

}
