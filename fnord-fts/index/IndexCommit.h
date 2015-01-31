/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXCOMMIT_H
#define INDEXCOMMIT_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Represents a single commit into an index as seen by the {@link IndexDeletionPolicy} or {@link IndexReader}.
///
/// Changes to the content of an index are made visible only after the writer who made that change commits by
/// writing a new segments file (segments_N).  This point in time, when the action of writing of a new segments
/// file to the directory is completed, is an index commit.
///
/// Each index commit point has a unique segments file associated with it. The segments file associated with a
/// later index commit point would have a larger N.
class IndexCommit : public LuceneObject {
public:
    virtual ~IndexCommit();

    LUCENE_CLASS(IndexCommit);

public:
    /// Get the segments file (segments_N) associated with this commit point.
    virtual String getSegmentsFileName() = 0;

    /// Returns all index files referenced by this commit point.
    virtual HashSet<String> getFileNames() = 0;

    /// Returns the {@link Directory} for the index.
    virtual DirectoryPtr getDirectory() = 0;

    /// Delete this commit point.  This only applies when using the commit point in the context of IndexWriter's
    /// IndexDeletionPolicy.
    ///
    /// Upon calling this, the writer is notified that this commit point should be deleted.
    ///
    /// Decision that a commit-point should be deleted is taken by the {@link IndexDeletionPolicy} in effect
    /// and therefore this should only be called by its {@link IndexDeletionPolicy#onInit onInit()} or
    /// {@link IndexDeletionPolicy#onCommit onCommit()} methods.
    virtual void deleteCommit() = 0;

    virtual bool isDeleted() = 0;

    /// Returns true if this commit is an optimized index.
    virtual bool isOptimized() = 0;

    /// Two IndexCommits are equal if both their Directory and versions are equal.
    virtual bool equals(const LuceneObjectPtr& other);

    virtual int32_t hashCode();

    /// Returns the version for this IndexCommit.  This is the same value that {@link IndexReader#getVersion}
    /// would return if it were opened on this commit.
    virtual int64_t getVersion() = 0;

    /// Returns the generation (the _N in segments_N) for this IndexCommit.
    virtual int64_t getGeneration() = 0;

    /// Convenience method that returns the last modified time of the segments_N file corresponding to this
    /// index commit, equivalent to getDirectory()->fileModified(getSegmentsFileName()).
    virtual int64_t getTimestamp();

    /// Returns userData, previously passed to {@link IndexWriter#commit(Map)} for this commit.  Map is
    /// String -> String.
    virtual MapStringString getUserData() = 0;
};

}

}
#endif
