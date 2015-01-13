/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FastCharStream.h"
#include "Reader.h"
#include "MiscUtils.h"

namespace Lucene {

FastCharStream::FastCharStream(const ReaderPtr& reader) {
    input = reader;
    bufferLength = 0;
    bufferPosition = 0;
    tokenStart = 0;
    bufferStart = 0;
}

FastCharStream::~FastCharStream() {
}

wchar_t FastCharStream::readChar() {
    if (bufferPosition >= bufferLength) {
        refill();
    }
    return buffer[bufferPosition++];
}

void FastCharStream::refill() {
    int32_t newPosition = bufferLength - tokenStart;

    if (tokenStart == 0) { // token won't fit in buffer
        if (!buffer) {
            buffer = CharArray::newInstance(2048);
        } else if (bufferLength == buffer.size()) { // grow buffer
            buffer.resize(buffer.size() * 2);
        }
    } else { // shift token to front
        MiscUtils::arrayCopy(buffer.get(), tokenStart, buffer.get(), 0, newPosition);
    }

    bufferLength = newPosition; // update state
    bufferPosition = newPosition;
    bufferStart += tokenStart;
    tokenStart = 0;

    int32_t charsRead = input->read(buffer.get(), newPosition, buffer.size() - newPosition); // fill space in buffer
    if (charsRead == -1) {
        boost::throw_exception(IOException(L"read past eof"));
    } else {
        bufferLength += charsRead;
    }
}

wchar_t FastCharStream::BeginToken() {
    tokenStart = bufferPosition;
    return readChar();
}

void FastCharStream::backup(int32_t amount) {
    bufferPosition -= amount;
}

String FastCharStream::GetImage() {
    return String(buffer.get() + tokenStart, bufferPosition - tokenStart);
}

CharArray FastCharStream::GetSuffix(int32_t length) {
    CharArray value(CharArray::newInstance(length));
    MiscUtils::arrayCopy(buffer.get(), bufferPosition - length, value.get(), 0, length);
    return value;
}

void FastCharStream::Done() {
    try {
        input->close();
    } catch (IOException&) {
        // ignore IO exceptions
    }
}

int32_t FastCharStream::getColumn() {
    return bufferStart + bufferPosition;
}

int32_t FastCharStream::getLine() {
    return 1;
}

int32_t FastCharStream::getEndColumn() {
    return bufferStart + bufferPosition;
}

int32_t FastCharStream::getEndLine() {
    return 1;
}

int32_t FastCharStream::getBeginColumn() {
    return bufferStart + tokenStart;
}

int32_t FastCharStream::getBeginLine() {
    return 1;
}

}
