/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXDELETIONPOLICY_H
#define INDEXDELETIONPOLICY_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Policy for deletion of stale {@link IndexCommit index commits}.
/// Implement this interface, and pass
/// it to one of the {@link IndexWriter} or {@link IndexReader} constructors, to customize when older
/// {@link IndexCommit point-in-time commits} are deleted from the index directory.  The default deletion
/// policy is {@link KeepOnlyLastCommitDeletionPolicy}, which always removes old commits as soon as a new
/// commit is done (this matches the behavior before 2.2).
///
/// One expected use case for this (and the reason why it was first created) is to work around problems
/// with an index directory accessed via filesystems like NFS because NFS does not provide the "delete on
/// last close" semantics that Lucene's "point in time" search normally relies on.  By implementing a
/// custom deletion policy, such as "a commit is only removed once it has been stale for more than X
/// minutes", you can give your readers time to refresh to the new commit before {@link IndexWriter}
/// removes the old commits.  Note that doing so will increase the storage requirements of the index.
class IndexDeletionPolicy : public LuceneObject {
protected:
    IndexDeletionPolicy();

public:
    virtual ~IndexDeletionPolicy();

    LUCENE_CLASS(IndexDeletionPolicy);

public:
    /// This is called once when a writer is first instantiated to give the policy a chance to remove old
    /// commit points.
    ///
    /// The writer locates all index commits present in the index directory and calls this method.  The
    /// policy may choose to delete some of the commit points, doing so by calling method {@link
    /// IndexCommit#delete delete()} of {@link IndexCommit}.
    ///
    /// Note: the last CommitPoint is the most recent one, ie. the "front index state". Be careful not to
    /// delete it, unless you know for sure what you are doing, and unless you can afford to lose the
    /// index content while doing that.
    ///
    /// @param commits List of current {@link IndexCommit point-in-time commits}, sorted by age (the 0th
    /// one is the oldest commit).
    virtual void onInit(Collection<IndexCommitPtr> commits) = 0;

    /// This is called each time the writer completed a commit.  This gives the policy a chance to remove
    /// old commit points with each commit.
    ///
    /// The policy may now choose to delete old commit points by calling method {@link
    /// IndexCommit#delete delete()} of {@link IndexCommit}.
    ///
    /// This method is only called when {@link IndexWriter#commit} or {@link IndexWriter#close} is called,
    /// or possibly not at all if the {@link IndexWriter#rollback} is called.
    ///
    /// Note: the last CommitPoint is the most recent one, ie. the "front index state". Be careful not to
    /// delete it, unless you know for sure what you are doing, and unless you can afford to lose the
    /// index content while doing that.
    ///
    /// @param commits List of {@link IndexCommit}, sorted by age (the 0th one is the oldest commit).
    virtual void onCommit(Collection<IndexCommitPtr> commits) = 0;
};

}

#endif
