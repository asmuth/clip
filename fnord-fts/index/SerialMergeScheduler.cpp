/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/index/SerialMergeScheduler.h"
#include "fnord-fts/index/IndexWriter.h"

namespace Lucene {

SerialMergeScheduler::~SerialMergeScheduler() {
}

void SerialMergeScheduler::merge(const IndexWriterPtr& writer) {
    SyncLock syncLock(this);
    while (true) {
        OneMergePtr merge(writer->getNextMerge());
        if (!merge) {
            break;
        }
        writer->merge(merge);
    }
}

void SerialMergeScheduler::close() {
}

}
