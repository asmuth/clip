/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermPositionVector.h"

namespace Lucene {

TermPositionVector::TermPositionVector() {
}

TermPositionVector::~TermPositionVector() {
}

Collection<int32_t> TermPositionVector::getTermPositions(int32_t index) {
    BOOST_ASSERT(false);
    return Collection<int32_t>(); // override
}

Collection<TermVectorOffsetInfoPtr> TermPositionVector::getOffsets(int32_t index) {
    BOOST_ASSERT(false);
    return Collection<TermVectorOffsetInfoPtr>(); // override
}

}
