/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ReusableStringReader.h"
#include "MiscUtils.h"

namespace Lucene {

ReusableStringReader::ReusableStringReader() {
    upto = 0;
    left = 0;
}

ReusableStringReader::~ReusableStringReader() {
}

void ReusableStringReader::init(const String& s) {
    this->s = s;
    left = s.length();
    this->upto = 0;
}

int32_t ReusableStringReader::read(wchar_t* buffer, int32_t offset, int32_t length) {
    if (left > length) {
        MiscUtils::arrayCopy(s.begin(), upto, buffer, offset, length);
        upto += length;
        left -= length;
        return length;
    } else if (left == 0) {
        s.clear();
        return -1;
    } else {
        MiscUtils::arrayCopy(s.begin(), upto, buffer, offset, left);
        int32_t r = left;
        left = 0;
        upto = s.length();
        return r;
    }
}

void ReusableStringReader::close() {
}

}
