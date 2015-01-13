/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "IndexCommit.h"

namespace Lucene {

KeepOnlyLastCommitDeletionPolicy::~KeepOnlyLastCommitDeletionPolicy() {
}

void KeepOnlyLastCommitDeletionPolicy::onInit(Collection<IndexCommitPtr> commits) {
    // Note that commits.size() should normally be 1
    onCommit(commits);
}

void KeepOnlyLastCommitDeletionPolicy::onCommit(Collection<IndexCommitPtr> commits) {
    // Note that commits.size() should normally be 2 (if not called by onInit above)
    int32_t size = commits.size();
    for (int32_t i = 0; i < size - 1; ++i) {
        commits[i]->deleteCommit();
    }
}

}
