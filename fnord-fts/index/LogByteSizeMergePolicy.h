/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOGBYTESIZEMERGEPOLICY_H
#define LOGBYTESIZEMERGEPOLICY_H

#include "fnord-fts/index/LogMergePolicy.h"

namespace fnord {
namespace fts {

/// This is a {@link LogMergePolicy} that measures size of a segment as the total byte size of the
/// segment's files.
class LogByteSizeMergePolicy : public LogMergePolicy {
public:
    LogByteSizeMergePolicy(const IndexWriterPtr& writer);
    virtual ~LogByteSizeMergePolicy();

    LUCENE_CLASS(LogByteSizeMergePolicy);

public:
    /// Default minimum segment size.  @see setMinMergeMB.
    static const double DEFAULT_MIN_MERGE_MB;

    /// Default maximum segment size.  A segment of this size or larger will never be merged.
    /// @see setMaxMergeMB
    static const double DEFAULT_MAX_MERGE_MB;

protected:
    virtual int64_t size(const SegmentInfoPtr& info);

public:
    /// Determines the largest segment (measured by total byte size of the segment's files, in MB)
    /// that may be merged with other segments.  Small values (eg., less than 50 MB) are best for
    /// interactive indexing, as this limits the length of pauses while indexing to a few seconds.
    /// Larger values are best for batched indexing and speedier searches.
    ///
    /// Note that {@link #setMaxMergeDocs} is also used to check whether a segment is too large for
    /// merging (it's either or).
    void setMaxMergeMB(double mb);

    /// Returns the largest segment (measured by total byte size of the segment's files, in MB) that
    /// may be merged with other segments. @see #setMaxMergeMB
    double getMaxMergeMB();

    /// Sets the minimum size for the lowest level segments.  Any segments below this size are
    /// considered to be on the same level (even if they vary drastically in size) and will be merged
    /// whenever there are mergeFactor of them.  This effectively truncates the "long tail" of small
    /// segments that would otherwise be created into a single level.  If you set this too large, it
    /// could greatly increase the merging cost during indexing (if you flush many small segments).
    void setMinMergeMB(double mb);

    /// Get the minimum size for a segment to remain un-merged.  @see #setMinMergeMB
    double getMinMergeMB();
};

}

}
#endif
