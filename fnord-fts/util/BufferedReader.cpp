/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BufferedReader.h"
#include "MiscUtils.h"

namespace Lucene {

const int32_t BufferedReader::READER_BUFFER = 8192;

BufferedReader::BufferedReader(const ReaderPtr& reader, int32_t size) {
    this->reader = reader;
    this->bufferSize = size;
    this->bufferLength = 0;
    this->bufferPosition = 0;
}

BufferedReader::~BufferedReader() {
}

int32_t BufferedReader::read() {
    if (bufferPosition >= bufferLength) {
        if (refill() == READER_EOF) {
            return READER_EOF;
        }
    }
    return buffer[bufferPosition++];
}

int32_t BufferedReader::peek() {
    if (bufferPosition >= bufferLength) {
        if (refill() == READER_EOF) {
            return READER_EOF;
        }
    }
    return buffer[bufferPosition];
}

int32_t BufferedReader::read(wchar_t* b, int32_t offset, int32_t length) {
    if (length == 0) {
        return 0;
    }

    int32_t remaining = length;

    while (remaining > 0) {
        int32_t available = bufferLength - bufferPosition;

        if (remaining <= available) {
            // the buffer contains enough data to satisfy this request
            MiscUtils::arrayCopy(buffer.get(), bufferPosition, b, offset, remaining);
            bufferPosition += remaining;
            remaining = 0;
        } else if (available > 0) {
            // the buffer does not have enough data, first serve all we've got
            MiscUtils::arrayCopy(buffer.get(), bufferPosition, b, offset, available);
            bufferPosition += available;
            offset += available;
            remaining -= available;
        } else if (refill() == READER_EOF) {
            length -= remaining;
            break;
        }
    }

    return length == 0 ? READER_EOF : length;
}

bool BufferedReader::readLine(String& line) {
    line.clear();
    wchar_t ch = (wchar_t)read();
    while (ch != (wchar_t)READER_EOF && ch != L'\r' && ch != L'\n') {
        line += ch;
        ch = (wchar_t)read();
    }
    if (ch == '\r' && (wchar_t)peek() == L'\n') {
        read();
    }
    return (!line.empty() || ch != (wchar_t)READER_EOF);
}

int32_t BufferedReader::refill() {
    if (!buffer) {
        buffer = CharArray::newInstance(bufferSize);    // allocate buffer lazily
    }
    int32_t readLength = reader->read(buffer.get(), 0, bufferSize);
    bufferLength = readLength == READER_EOF ? 0 : readLength;
    bufferPosition = 0;
    return readLength;
}

void BufferedReader::close() {
    reader->close();
    bufferLength = 0;
    bufferPosition = 0;
}

bool BufferedReader::markSupported() {
    return false;
}

void BufferedReader::reset() {
    reader->reset();
    bufferLength = 0;
    bufferPosition = 0;
}

}
