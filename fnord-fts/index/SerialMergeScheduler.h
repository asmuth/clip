/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SERIALMERGESCHEDULER_H
#define SERIALMERGESCHEDULER_H

#include "fnord-fts/index/MergeScheduler.h"

namespace fnord {
namespace fts {

/// A {@link MergeScheduler} that simply does each merge sequentially, using the current thread.
class SerialMergeScheduler : public MergeScheduler {
public:
    virtual ~SerialMergeScheduler();

    LUCENE_CLASS(SerialMergeScheduler);

public:
    /// Just do the merges in sequence. We do this "synchronized" so that even if the application is using
    /// multiple threads, only one merge may run at a time.
    virtual void merge(const IndexWriterPtr& writer);

    /// Close this MergeScheduler.
    virtual void close();
};

}

}
#endif
