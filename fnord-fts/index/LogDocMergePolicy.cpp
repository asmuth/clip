/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LogDocMergePolicy.h"

namespace Lucene {

/// Default minimum segment size.  @see setMinMergeDocs
const int32_t LogDocMergePolicy::DEFAULT_MIN_MERGE_DOCS = 1000;

LogDocMergePolicy::LogDocMergePolicy(const IndexWriterPtr& writer) : LogMergePolicy(writer) {
    minMergeSize = DEFAULT_MIN_MERGE_DOCS;

    // maxMergeSize is never used by LogDocMergePolicy; set it to LLONG_MAX to disable it
    maxMergeSize = std::numeric_limits<int64_t>::max();
}

LogDocMergePolicy::~LogDocMergePolicy() {
}

int64_t LogDocMergePolicy::size(const SegmentInfoPtr& info) {
    return sizeDocs(info);
}

void LogDocMergePolicy::setMinMergeDocs(int32_t minMergeDocs) {
    minMergeSize = minMergeDocs;
}

int32_t LogDocMergePolicy::getMinMergeDocs() {
    return (int32_t)minMergeSize;
}

}
