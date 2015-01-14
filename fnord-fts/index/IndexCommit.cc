/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/index/IndexCommit.h"
#include "fnord-fts/store/Directory.h"

namespace fnord {
namespace fts {

IndexCommit::~IndexCommit() {
}

bool IndexCommit::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    IndexCommitPtr otherCommit(std::dynamic_pointer_cast<IndexCommit>(other));
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

}
