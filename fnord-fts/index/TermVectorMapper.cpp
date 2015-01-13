/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorMapper.h"

namespace Lucene {

TermVectorMapper::TermVectorMapper(bool ignoringPositions, bool ignoringOffsets) {
    this->ignoringPositions = ignoringPositions;
    this->ignoringOffsets = ignoringOffsets;
}

TermVectorMapper::~TermVectorMapper() {
}

bool TermVectorMapper::isIgnoringPositions() {
    return ignoringPositions;
}

bool TermVectorMapper::isIgnoringOffsets() {
    return ignoringOffsets;
}

void TermVectorMapper::setDocumentNumber(int32_t documentNumber) {
    // override
}

}
