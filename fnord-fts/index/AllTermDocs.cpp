/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/index//LuceneInc.h"
#include "fnord-fts/index/AllTermDocs.h"
#include "fnord-fts/index/SegmentReader.h"
#include "fnord-fts/util/BitVector.h"

namespace Lucene {

AllTermDocs::AllTermDocs(const SegmentReaderPtr& parent) : AbstractAllTermDocs(parent->maxDoc()) {
    SyncLock parentLock(parent);
    this->_deletedDocs = parent->deletedDocs;
}

AllTermDocs::~AllTermDocs() {
}

bool AllTermDocs::isDeleted(int32_t doc) {
    BitVectorPtr deletedDocs(_deletedDocs.lock());
    return (deletedDocs && deletedDocs->get(_doc));
}

}
