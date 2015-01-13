/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "RawPostingList.h"
#include "DocumentsWriter.h"

namespace Lucene {

const int32_t RawPostingList::BYTES_SIZE = DocumentsWriter::OBJECT_HEADER_BYTES + 3 * DocumentsWriter::INT_NUM_BYTE;

RawPostingList::RawPostingList() {
    textStart = 0;
    intStart = 0;
    byteStart = 0;
}

RawPostingList::~RawPostingList() {
}

}
