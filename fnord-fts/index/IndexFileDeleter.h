/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXFILEDELETER_H
#define INDEXFILEDELETER_H

#include "IndexCommit.h"

namespace Lucene {

/// This class keeps track of each SegmentInfos instance that is still "live", either because it corresponds to a
/// segments_N file in the Directory (a "commit", ie. a committed SegmentInfos) or because it's an in-memory
/// SegmentInfos that a writer is actively updating but has not yet committed.  This class uses simple reference
/// counting to map the live SegmentInfos instances to individual files in the Directory.
///
/// The same directory file may be referenced by more than one IndexCommit, i.e. more than one SegmentInfos.
/// Therefore we count how many commits reference each file.  When all the commits referencing a certain file have
/// been deleted, the refcount for that file becomes zero, and the file is deleted.
///
/// A separate deletion policy interface (IndexDeletionPolicy) is consulted on creation (onInit) and once per
/// commit (onCommit), to decide when a commit should be removed.
///
/// It is the business of the IndexDeletionPolicy to choose when to delete commit points.  The actual mechanics of
/// file deletion, retrying, etc, derived from the deletion of commit points is the business of the IndexFileDeleter.
///
/// The current default deletion policy is {@link KeepOnlyLastCommitDeletionPolicy}, which removes all prior commits
/// when a new commit has completed.  This matches the behavior before 2.2.
///
/// Note that you must hold the write.lock before instantiating this class.  It opens segments_N file(s) directly
/// with no retry logic.
class IndexFileDeleter : public LuceneObject {
public:
    /// Initialize the deleter: find all previous commits in the Directory, incref the files they reference, call
    /// the policy to let it delete commits.  This will remove any files not referenced by any of the commits.
    IndexFileDeleter(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& policy, const SegmentInfosPtr& segmentInfos, const InfoStreamPtr& infoStream, const DocumentsWriterPtr& docWriter, HashSet<String> synced);
    virtual ~IndexFileDeleter();

    LUCENE_CLASS(IndexFileDeleter);

protected:
    /// Files that we tried to delete but failed (likely because they are open and we are running on Windows),
    /// so we will retry them again later
    HashSet<String> deletable;

    /// Reference count for all files in the index.  Counts how many existing commits reference a file.
    MapStringRefCount refCounts;

    /// Holds all commits (segments_N) currently in the index.  This will have just 1 commit if you are using the
    /// default delete policy (KeepOnlyLastCommitDeletionPolicy).  Other policies may leave commit points live for
    /// longer in which case this list would be longer than 1
    Collection<IndexCommitPtr> commits;

    /// Holds files we had incref'd from the previous non-commit checkpoint
    Collection< HashSet<String> > lastFiles;

    /// Commits that the IndexDeletionPolicy have decided to delete
    Collection<CommitPointPtr> commitsToDelete;

    InfoStreamPtr infoStream;
    DirectoryPtr directory;
    IndexDeletionPolicyPtr policy;
    DocumentsWriterPtr docWriter;

    SegmentInfosPtr lastSegmentInfos;
    HashSet<String> synced;

    /// Change to true to see details of reference counts when infoStream != null
    static bool VERBOSE_REF_COUNTS;

public:
    bool startingCommitDeleted;

protected:
    void message(const String& message);

    /// Remove the CommitPoints in the commitsToDelete List by DecRef'ing all files from each SegmentInfos.
    void deleteCommits();

    void deletePendingFiles();

    RefCountPtr getRefCount(const String& fileName);

public:
    void setInfoStream(const InfoStreamPtr& infoStream);

    SegmentInfosPtr getLastSegmentInfos();

    /// Writer calls this when it has hit an error and had to roll back, to tell us that there may now be
    /// unreferenced files in the filesystem.  So we re-list the filesystem and delete such files.  If
    /// segmentName is non-null, we will only delete files corresponding to that segment.
    void refresh(const String& segmentName);
    void refresh();

    void close();

    /// For definition of "check point" see IndexWriter comments: "Clarification: Check Points (and commits)".
    /// Writer calls this when it has made a "consistent change" to the index, meaning new files are written to
    /// the index and the in-memory SegmentInfos have been modified to point to those files.
    ///
    /// This may or may not be a commit (segments_N may or may not have been written).
    ///
    /// We simply incref the files referenced by the new SegmentInfos and decref the files we had previously
    /// seen (if any).
    ///
    /// If this is a commit, we also call the policy to give it a chance to remove other commits.  If any
    /// commits are removed, we decref their files as well.
    void checkpoint(const SegmentInfosPtr& segmentInfos, bool isCommit);

    void incRef(const SegmentInfosPtr& segmentInfos, bool isCommit);
    void incRef(HashSet<String> files);
    void incRef(const String& fileName);
    void decRef(HashSet<String> files);
    void decRef(const String& fileName);
    void decRef(const SegmentInfosPtr& segmentInfos);

    bool exists(const String& fileName);

    void deleteFiles(HashSet<String> files);

    /// Deletes the specified files, but only if they are new (have not yet been incref'd).
    void deleteNewFiles(HashSet<String> files);

    void deleteFile(const String& fileName);
};

/// Tracks the reference count for a single index file
class RefCount : public LuceneObject {
public:
    RefCount(const String& fileName);
    virtual ~RefCount();

    LUCENE_CLASS(RefCount);

public:
    String fileName; // fileName used only for better assert error messages
    bool initDone;
    int32_t count;

public:
    int32_t IncRef();
    int32_t DecRef();
};

/// Holds details for each commit point.  This class is also passed to the deletion policy.  Note: this class
/// has a natural ordering that is inconsistent with equals.
class CommitPoint : public IndexCommit {
public:
    CommitPoint(Collection<CommitPointPtr> commitsToDelete, const DirectoryPtr& directory, const SegmentInfosPtr& segmentInfos);
    virtual ~CommitPoint();

    LUCENE_CLASS(CommitPoint);

public:
    int64_t gen;
    HashSet<String> files;
    String segmentsFileName;
    bool deleted;
    DirectoryPtr directory;
    Collection<CommitPointPtr> commitsToDelete;
    int64_t version;
    int64_t generation;
    bool _isOptimized;
    MapStringString userData;

public:
    virtual String toString();

    /// Returns true if this commit is an optimized index.
    virtual bool isOptimized();

    /// Get the segments file (segments_N) associated with this commit point.
    virtual String getSegmentsFileName();

    /// Returns all index files referenced by this commit point.
    virtual HashSet<String> getFileNames();

    /// Returns the {@link Directory} for the index.
    virtual DirectoryPtr getDirectory();

    /// Returns the version for this IndexCommit.
    virtual int64_t getVersion();

    /// Returns the generation (the _N in segments_N) for this IndexCommit.
    virtual int64_t getGeneration();

    /// Returns userData, previously passed to {@link IndexWriter#commit(Map)} for this commit.
    virtual MapStringString getUserData();

    /// Called only be the deletion policy, to remove this commit point from the index.
    virtual void deleteCommit();

    virtual bool isDeleted();

    virtual int32_t compareTo(const LuceneObjectPtr& other);
};

}

#endif
