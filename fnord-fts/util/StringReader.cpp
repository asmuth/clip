/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StringReader.h"

namespace Lucene {

StringReader::StringReader(const String& str) {
    this->str = str;
    this->position = 0;
}

StringReader::~StringReader() {
}

int32_t StringReader::read() {
    return position == (int32_t)str.length() ? READER_EOF : (int32_t)str[position++];
}

int32_t StringReader::read(wchar_t* buffer, int32_t offset, int32_t length) {
    if (position >= (int32_t)str.length()) {
        return READER_EOF;
    }
    int32_t readChars = std::min(length, (int32_t)str.length() - position);
    std::wcsncpy(buffer + offset, str.c_str() + position, readChars);
    position += readChars;
    return readChars;
}

void StringReader::close() {
    str.clear();
}

bool StringReader::markSupported() {
    return false;
}

void StringReader::reset() {
    position = 0;
}

int64_t StringReader::length() {
    return str.length();
}

}
