/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOGDOCMERGEPOLICY_H
#define LOGDOCMERGEPOLICY_H

#include "LogMergePolicy.h"

namespace Lucene {

/// This is a {@link LogMergePolicy} that measures size of a segment as the number of documents
/// (not taking deletions into account).
class LogDocMergePolicy : public LogMergePolicy {
public:
    LogDocMergePolicy(const IndexWriterPtr& writer);
    virtual ~LogDocMergePolicy();

    LUCENE_CLASS(LogDocMergePolicy);

public:
    /// Default minimum segment size.  @see setMinMergeDocs
    static const int32_t DEFAULT_MIN_MERGE_DOCS;

protected:
    virtual int64_t size(const SegmentInfoPtr& info);

public:
    /// Sets the minimum size for the lowest level segments. Any segments below this size are considered
    /// to be on the same level (even if they vary drastically in size) and will be merged whenever there
    /// are mergeFactor of them.  This effectively truncates the "long tail" of small segments that would
    /// otherwise be created into a single level.  If you set this too large, it could greatly increase the
    /// merging cost during indexing (if you flush many small segments).
    void setMinMergeDocs(int32_t minMergeDocs);

    /// Get the minimum size for a segment to remain un-merged. @see #setMinMergeDocs
    int32_t getMinMergeDocs();
};

}

#endif
