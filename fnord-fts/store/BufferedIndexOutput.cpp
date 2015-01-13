/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BufferedIndexOutput.h"
#include "MiscUtils.h"

namespace Lucene {

const int32_t BufferedIndexOutput::BUFFER_SIZE = 16384;

BufferedIndexOutput::BufferedIndexOutput() {
    bufferStart = 0;
    bufferPosition = 0;
    buffer = ByteArray::newInstance(BUFFER_SIZE);
}

BufferedIndexOutput::~BufferedIndexOutput() {
}

void BufferedIndexOutput::writeByte(uint8_t b) {
    if (bufferPosition >= BUFFER_SIZE) {
        flush();
    }
    buffer[bufferPosition++] = b;
}

void BufferedIndexOutput::writeBytes(const uint8_t* b, int32_t offset, int32_t length) {
    int32_t bytesLeft = BUFFER_SIZE - bufferPosition;
    if (bytesLeft >= length) {
        // we add the data to the end of the buffer
        MiscUtils::arrayCopy(b, offset, buffer.get(), bufferPosition, length);
        bufferPosition += length;
        // if the buffer is full, flush it
        if (BUFFER_SIZE - bufferPosition == 0) {
            flush();
        }
    } else if (length > BUFFER_SIZE) {
        // we flush the buffer
        if (bufferPosition > 0) {
            flush();
        }
        // and write data at once
        flushBuffer(b, offset, length);
        bufferStart += length;
    } else {
        // we fill/flush the buffer (until the input is written)
        int32_t pos = 0; // position in the input data
        int32_t pieceLength;
        while (pos < length) {
            pieceLength = (length - pos < bytesLeft) ? length - pos : bytesLeft;
            MiscUtils::arrayCopy(b, pos + offset, buffer.get(), bufferPosition, pieceLength);
            pos += pieceLength;
            bufferPosition += pieceLength;
            // if the buffer is full, flush it
            bytesLeft = BUFFER_SIZE - bufferPosition;
            if (bytesLeft == 0) {
                flush();
                bytesLeft = BUFFER_SIZE;
            }
        }
    }
}

void BufferedIndexOutput::flush() {
    flushBuffer(buffer.get(), bufferPosition);
    bufferStart += bufferPosition;
    bufferPosition = 0;
}

void BufferedIndexOutput::flushBuffer(const uint8_t* b, int32_t length) {
    flushBuffer(b, 0, length);
}

void BufferedIndexOutput::flushBuffer(const uint8_t* b, int32_t offset, int32_t length) {
    // override
}

void BufferedIndexOutput::close() {
    flush();
}

int64_t BufferedIndexOutput::getFilePointer() {
    return bufferStart + bufferPosition;
}

void BufferedIndexOutput::seek(int64_t pos) {
    flush();
    bufferStart = pos;
}

}
