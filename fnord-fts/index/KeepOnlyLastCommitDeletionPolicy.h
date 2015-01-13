/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef KEEPONLYLASTCOMMITDELETIONPOLICY_H
#define KEEPONLYLASTCOMMITDELETIONPOLICY_H

#include "fnord-fts/index/IndexDeletionPolicy.h"

namespace Lucene {

/// This {@link IndexDeletionPolicy} implementation that keeps only the most recent commit and immediately
/// removes all prior commits after a new commit is done.  This is the default deletion policy.
class KeepOnlyLastCommitDeletionPolicy : public IndexDeletionPolicy {
public:
    virtual ~KeepOnlyLastCommitDeletionPolicy();

    LUCENE_CLASS(KeepOnlyLastCommitDeletionPolicy);

public:
    /// Deletes all commits except the most recent one.
    virtual void onInit(Collection<IndexCommitPtr> commits);

    /// Deletes all commits except the most recent one.
    virtual void onCommit(Collection<IndexCommitPtr> commits);
};

}

#endif
