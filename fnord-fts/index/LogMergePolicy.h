/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOGMERGEPOLICY_H
#define LOGMERGEPOLICY_H

#include "fnord-fts/index/MergePolicy.h"

namespace Lucene {

/// This class implements a {@link MergePolicy} that tries to merge segments into levels of exponentially
/// increasing size, where each level has fewer segments than the value of the merge factor. Whenever extra
/// segments (beyond the merge factor upper bound) are encountered, all segments within the level are merged.
/// You can get or set the merge factor using {@link #getMergeFactor()} and {@link #setMergeFactor(int)}
/// respectively.
///
/// This class is abstract and requires a subclass to define the {@link #size} method which specifies how a
/// segment's size is determined.  {@link LogDocMergePolicy} is one subclass that measures size by document
/// count in the segment.  {@link LogByteSizeMergePolicy} is another subclass that measures size as the total
/// byte size of the file(s) for the segment.
class LogMergePolicy : public MergePolicy {
public:
    LogMergePolicy(const IndexWriterPtr& writer);
    virtual ~LogMergePolicy();

    LUCENE_CLASS(LogMergePolicy);

protected:
    int32_t mergeFactor;

    double noCFSRatio;

    bool calibrateSizeByDeletes;
    bool _useCompoundFile;
    bool _useCompoundDocStore;

public:
    /// Defines the allowed range of log(size) for each level.  A level is computed by taking the max segment
    /// log size, minus LEVEL_LOG_SPAN, and finding all segments falling within that range.
    static const double LEVEL_LOG_SPAN;

    /// Default merge factor, which is how many segments are merged at a time.
    static const int32_t DEFAULT_MERGE_FACTOR;

    /// Default maximum segment size.  A segment of this size or larger will never be merged.
    /// @see setMaxMergeDocs
    static const int32_t DEFAULT_MAX_MERGE_DOCS;

    /// Default noCFSRatio.  If a merge's size is >= 10% of the index, then we disable compound file for it.
    /// @see #setNoCFSRatio
    static const double DEFAULT_NO_CFS_RATIO;

    int64_t minMergeSize;
    int64_t maxMergeSize;
    int32_t maxMergeDocs;

public:
    /// @see #setNoCFSRatio
    double getNoCFSRatio();

    /// If a merged segment will be more than this percentage of the total size of the index, leave the segment as
    /// non-compound file even if compound file is enabled.  Set to 1.0 to always use CFS regardless of merge size.
    void setNoCFSRatio(double noCFSRatio);

    /// Returns the number of segments that are merged at once and also controls the total number of segments
    /// allowed to accumulate in the index.
    int32_t getMergeFactor();

    /// Determines how often segment indices are merged by addDocument().  With smaller values, less RAM is
    /// used while indexing, and searches on unoptimized indices are faster, but indexing speed is slower.
    /// With larger values, more RAM is used during indexing, and while searches on unoptimized indices are
    /// slower, indexing is faster.  Thus larger values (> 10) are best for batch index creation, and smaller
    /// values (< 10) for indices that are interactively maintained.
    void setMergeFactor(int32_t mergeFactor);

    /// Returns true if a newly flushed (not from merge) segment should use the compound file format.
    virtual bool useCompoundFile(const SegmentInfosPtr& segments, const SegmentInfoPtr& newSegment);

    /// Sets whether compound file format should be used for newly flushed and newly merged segments.
    void setUseCompoundFile(bool useCompoundFile);

    /// Returns true if newly flushed and newly merge segments are written in compound file format.
    /// @see #setUseCompoundFile
    bool getUseCompoundFile();

    /// Returns true if the doc store files should use the compound file format.
    virtual bool useCompoundDocStore(const SegmentInfosPtr& segments);

    /// Sets whether compound file format should be used for newly flushed and newly merged doc store
    /// segment files (term vectors and stored fields).
    void setUseCompoundDocStore(bool useCompoundDocStore);

    /// Returns true if newly flushed and newly merge doc store segment files (term vectors and stored fields)
    /// are written in compound file format. @see #setUseCompoundDocStore
    bool getUseCompoundDocStore();

    /// Sets whether the segment size should be calibrated by the number of deletes when choosing segments
    /// for merge.
    void setCalibrateSizeByDeletes(bool calibrateSizeByDeletes);

    /// Returns true if the segment size should be calibrated by the number of deletes when choosing segments
    /// for merge.
    bool getCalibrateSizeByDeletes();

    /// Release all resources for the policy.
    virtual void close();

    /// Returns the merges necessary to optimize the index.  This merge policy defines "optimized" to mean only
    /// one segment in the index, where that segment has no deletions pending nor separate norms, and it is in
    /// compound file format if the current useCompoundFile setting is true.  This method returns multiple merges
    /// (mergeFactor at a time) so the {@link MergeScheduler} in use may make use of concurrency.
    virtual MergeSpecificationPtr findMergesForOptimize(const SegmentInfosPtr& segmentInfos, int32_t maxSegmentCount, SetSegmentInfo segmentsToOptimize);

    /// Finds merges necessary to expunge all deletes from the index.  We simply merge adjacent segments that have
    /// deletes, up to mergeFactor at a time.
    virtual MergeSpecificationPtr findMergesToExpungeDeletes(const SegmentInfosPtr& segmentInfos);

    /// Checks if any merges are now necessary and returns a {@link MergePolicy.MergeSpecification} if so.  A merge
    /// is necessary when there are more than {@link #setMergeFactor} segments at a given level.  When multiple
    /// levels have too many segments, this method will return multiple merges, allowing the {@link MergeScheduler}
    /// to use concurrency.
    virtual MergeSpecificationPtr findMerges(const SegmentInfosPtr& segmentInfos);

    /// Determines the largest segment (measured by document count) that may be merged with other segments.
    /// Small values (eg., less than 10,000) are best for interactive indexing, as this limits the length of
    /// pauses while indexing to a few seconds.  Larger values are best for batched indexing and speedier searches.
    ///
    /// The default value is INT_MAX.
    ///
    /// The default merge policy ({@link LogByteSizeMergePolicy}) also allows you to set this limit by net size
    /// (in MB) of the segment, using {@link LogByteSizeMergePolicy#setMaxMergeMB}.
    void setMaxMergeDocs(int32_t maxMergeDocs);

    /// Returns the largest segment (measured by document count) that may be merged with other segments.
    /// @see #setMaxMergeDocs
    int32_t getMaxMergeDocs();

protected:
    bool verbose();
    void message(const String& message);

    virtual int64_t size(const SegmentInfoPtr& info) = 0;

    int64_t sizeDocs(const SegmentInfoPtr& info);
    int64_t sizeBytes(const SegmentInfoPtr& info);

    bool isOptimized(const SegmentInfosPtr& infos, int32_t maxNumSegments, SetSegmentInfo segmentsToOptimize);

    /// Returns true if this single info is optimized (has no pending norms or deletes, is in the same dir as the
    /// writer, and matches the current compound file setting
    bool isOptimized(const SegmentInfoPtr& info);

    OneMergePtr makeOneMerge(const SegmentInfosPtr& infos, const SegmentInfosPtr& infosToMerge);
};

}

#endif
