/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CharReader.h"

namespace Lucene {

CharReader::CharReader(const ReaderPtr& in) {
    input = in;
}

CharReader::~CharReader() {
}

CharStreamPtr CharReader::get(const ReaderPtr& input) {
    CharStreamPtr charStream(boost::dynamic_pointer_cast<CharStream>(input));
    return charStream ? charStream : newLucene<CharReader>(input);
}

int32_t CharReader::correctOffset(int32_t currentOff) {
    return currentOff;
}

void CharReader::close() {
    if (input) {
        input->close();
    }
}

int32_t CharReader::read(wchar_t* buffer, int32_t offset, int32_t length) {
    return input->read(buffer, offset, length);
}

bool CharReader::markSupported() {
    return input->markSupported();
}

void CharReader::mark(int32_t readAheadLimit) {
    input->mark(readAheadLimit);
}

void CharReader::reset() {
    input->reset();
}

}
