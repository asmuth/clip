/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "RAMInputStream.h"
#include "RAMFile.h"
#include "RAMOutputStream.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t RAMInputStream::BUFFER_SIZE = RAMOutputStream::BUFFER_SIZE;

RAMInputStream::RAMInputStream() {
    _length = 0;

    // make sure that we switch to the first needed buffer lazily
    currentBufferIndex = -1;
    bufferPosition = 0;
    bufferStart = 0;
    bufferLength = 0;
}

RAMInputStream::RAMInputStream(const RAMFilePtr& f) {
    file = f;
    _length = file->length;
    if (_length / BUFFER_SIZE >= INT_MAX) {
        boost::throw_exception(IOException(L"Too large RAMFile: " + StringUtils::toString(_length)));
    }

    // make sure that we switch to the first needed buffer lazily
    currentBufferIndex = -1;
    bufferPosition = 0;
    bufferStart = 0;
    bufferLength = 0;
}

RAMInputStream::~RAMInputStream() {
}

void RAMInputStream::close() {
    // nothing to do here
}

int64_t RAMInputStream::length() {
    return _length;
}

uint8_t RAMInputStream::readByte() {
    if (bufferPosition >= bufferLength) {
        ++currentBufferIndex;
        switchCurrentBuffer(true);
    }
    return currentBuffer[bufferPosition++];
}

void RAMInputStream::readBytes(uint8_t* b, int32_t offset, int32_t length) {
    while (length > 0) {
        if (bufferPosition >= bufferLength) {
            ++currentBufferIndex;
            switchCurrentBuffer(true);
        }

        int32_t remainInBuffer = bufferLength - bufferPosition;
        int32_t bytesToCopy = length < remainInBuffer ? length : remainInBuffer;
        MiscUtils::arrayCopy(currentBuffer.get(), bufferPosition, b, offset, bytesToCopy);
        offset += bytesToCopy;
        length -= bytesToCopy;
        bufferPosition += bytesToCopy;
    }
}

void RAMInputStream::switchCurrentBuffer(bool enforceEOF) {
    if (currentBufferIndex >= file->numBuffers()) {
        // end of file reached, no more buffers left
        if (enforceEOF) {
            boost::throw_exception(IOException(L"Read past EOF"));
        } else {
            // force eof if a read takes place at this position
            --currentBufferIndex;
            bufferPosition = BUFFER_SIZE;
        }
    } else {
        currentBuffer = file->getBuffer(currentBufferIndex);
        bufferPosition = 0;
        bufferStart = (int64_t)BUFFER_SIZE * (int64_t)currentBufferIndex;
        int64_t buflen = _length - bufferStart;
        bufferLength = buflen > BUFFER_SIZE ? BUFFER_SIZE : (int32_t)buflen;
    }
}

int64_t RAMInputStream::getFilePointer() {
    return currentBufferIndex < 0 ? 0 : bufferStart + bufferPosition;
}

void RAMInputStream::seek(int64_t pos) {
    if (!currentBuffer || (int32_t)pos < bufferStart || (int32_t)pos >= bufferStart + BUFFER_SIZE) {
        currentBufferIndex = (int32_t)(pos / BUFFER_SIZE);
        switchCurrentBuffer(false);
    }
    bufferPosition = (int32_t)(pos % BUFFER_SIZE);
}

LuceneObjectPtr RAMInputStream::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = IndexInput::clone(other ? other : newLucene<RAMInputStream>());
    RAMInputStreamPtr cloneInputStream(boost::dynamic_pointer_cast<RAMInputStream>(clone));
    cloneInputStream->file = file;
    cloneInputStream->_length = _length;
    cloneInputStream->currentBuffer = currentBuffer;
    cloneInputStream->currentBufferIndex = currentBufferIndex;
    cloneInputStream->bufferPosition = bufferPosition;
    cloneInputStream->bufferStart = bufferStart;
    cloneInputStream->bufferLength = bufferLength;
    return cloneInputStream;
}

}
