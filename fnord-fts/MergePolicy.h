/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MERGEPOLICY_H
#define MERGEPOLICY_H

#include "SegmentInfos.h"

namespace Lucene {

/// A MergePolicy determines the sequence of primitive merge operations to be used for overall merge
/// and optimize operations.
///
/// Whenever the segments in an index have been altered by {@link IndexWriter}, either the addition of
/// a newly flushed segment, addition of many segments from addIndexes* calls, or a previous merge that
/// may now need to cascade, {@link IndexWriter} invokes {@link #findMerges} to give the MergePolicy a
/// chance to pick merges that are now required.  This method returns a {@link MergeSpecification}
/// instance describing the set of merges that should be done, or null if no merges are necessary.
/// When IndexWriter.optimize is called, it calls {@link #findMergesForOptimize} and the MergePolicy
/// should then return the necessary merges.
///
/// Note that the policy can return more than one merge at a time.  In this case, if the writer is using
/// {@link SerialMergeScheduler}, the merges will be run sequentially but if it is using {@link
/// ConcurrentMergeScheduler} they will be run concurrently.
///
/// The default MergePolicy is {@link LogByteSizeMergePolicy}.
///
/// NOTE: This API is new and still experimental (subject to change suddenly in the next release)
class MergePolicy : public LuceneObject {
public:
    MergePolicy(const IndexWriterPtr& writer);
    virtual ~MergePolicy();

    LUCENE_CLASS(MergePolicy);

protected:
    IndexWriterWeakPtr _writer;

public:
    /// Determine what set of merge operations are now necessary on the index. {@link IndexWriter} calls
    /// this whenever there is a change to the segments. This call is always synchronized on the {@link
    /// IndexWriter} instance so only one thread at a time will call this method.
    /// @param segmentInfos the total set of segments in the index
    virtual MergeSpecificationPtr findMerges(const SegmentInfosPtr& segmentInfos) = 0;

    /// Determine what set of merge operations is necessary in order to optimize the index. {@link
    /// IndexWriter} calls this when its {@link IndexWriter#optimize()} method is called. This call is
    /// always synchronized on the {@link IndexWriter} instance so only one thread at a time will call
    /// this method.
    /// @param segmentInfos the total set of segments in the index
    /// @param maxSegmentCount requested maximum number of segments in the index (currently this is always 1)
    /// @param segmentsToOptimize contains the specific SegmentInfo instances that must be merged away.
    /// This may be a subset of all SegmentInfos.
    virtual MergeSpecificationPtr findMergesForOptimize(const SegmentInfosPtr& segmentInfos, int32_t maxSegmentCount, SetSegmentInfo segmentsToOptimize) = 0;

    /// Determine what set of merge operations is necessary in order to expunge all deletes from the index.
    /// @param segmentInfos the total set of segments in the index
    virtual MergeSpecificationPtr findMergesToExpungeDeletes(const SegmentInfosPtr& segmentInfos) = 0;

    /// Release all resources for the policy.
    virtual void close() = 0;

    /// Returns true if a newly flushed (not from merge) segment should use the compound file format.
    virtual bool useCompoundFile(const SegmentInfosPtr& segments, const SegmentInfoPtr& newSegment) = 0;

    /// Returns true if the doc store files should use the compound file format.
    virtual bool useCompoundDocStore(const SegmentInfosPtr& segments) = 0;
};

/// OneMerge provides the information necessary to perform  an individual primitive merge operation,
/// resulting in a single new segment.  The merge spec includes the subset of segments to be merged
/// as well as whether the new segment should use the compound file format.
class OneMerge : public LuceneObject {
public:
    OneMerge(const SegmentInfosPtr& segments, bool useCompoundFile);
    virtual ~OneMerge();

    LUCENE_CLASS(OneMerge);

public:
    SegmentInfoPtr info; // used by IndexWriter
    bool mergeDocStores; // used by IndexWriter
    bool optimize; // used by IndexWriter
    bool registerDone; // used by IndexWriter
    int64_t mergeGen; // used by IndexWriter
    bool isExternal; // used by IndexWriter
    int32_t maxNumSegmentsOptimize; // used by IndexWriter
    Collection<SegmentReaderPtr> readers; // used by IndexWriter
    Collection<SegmentReaderPtr> readersClone; // used by IndexWriter

    SegmentInfosPtr segments;
    bool useCompoundFile;
    bool aborted;
    LuceneException error;

public:
    /// Record that an exception occurred while executing this merge
    void setException(const LuceneException& error);

    /// Retrieve previous exception set by {@link #setException}.
    LuceneException getException();

    /// Mark this merge as aborted.  If this is called before the merge is committed then the merge will not be committed.
    void abort();

    /// Returns true if this merge was aborted.
    bool isAborted();

    void checkAborted(const DirectoryPtr& dir);

    String segString(const DirectoryPtr& dir);
};

/// A MergeSpecification instance provides the information necessary to perform multiple merges.
/// It simply contains a list of {@link OneMerge} instances.
class MergeSpecification : public LuceneObject {
public:
    MergeSpecification();
    virtual ~MergeSpecification();

    LUCENE_CLASS(MergeSpecification);

public:
    Collection<OneMergePtr> merges;

public:
    void add(const OneMergePtr& merge);
    String segString(const DirectoryPtr& dir);
};

}

#endif
