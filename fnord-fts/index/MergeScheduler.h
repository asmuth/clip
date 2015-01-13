/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MERGESCHEDULER_H
#define MERGESCHEDULER_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// {@link IndexWriter} uses an instance implementing this interface to execute the merges
/// selected by a {@link MergePolicy}.  The default MergeScheduler is {@link ConcurrentMergeScheduler}.
class MergeScheduler : public LuceneObject {
public:
    virtual ~MergeScheduler();

    LUCENE_CLASS(MergeScheduler);

public:
    /// Run the merges provided by {@link IndexWriter#getNextMerge()}.
    virtual void merge(const IndexWriterPtr& writer) = 0;

    /// Close this MergeScheduler.
    virtual void close() = 0;
};

}

#endif
