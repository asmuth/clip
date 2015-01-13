/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Reader.h"

namespace Lucene {

const int32_t Reader::READER_EOF = -1;

Reader::Reader() {
}

Reader::~Reader() {
}

int32_t Reader::read() {
    wchar_t buffer;
    return read(&buffer, 0, 1) == READER_EOF ? READER_EOF : buffer;
}

int64_t Reader::skip(int64_t n) {
    return 0; // override
}

bool Reader::markSupported() {
    return false; // override
}

void Reader::mark(int32_t readAheadLimit) {
    // override
}

void Reader::reset() {
    // override
}

int64_t Reader::length() {
    return 0; // override
}

}
