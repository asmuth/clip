/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "RAMOutputStream.h"
#include "RAMFile.h"
#include "RAMDirectory.h"
#include "MiscUtils.h"

namespace Lucene {

const int32_t RAMOutputStream::BUFFER_SIZE = 1024;

RAMOutputStream::RAMOutputStream() {
    file = newLucene<RAMFile>(RAMDirectoryPtr());

    // make sure that we switch to the first needed buffer lazily
    currentBufferIndex = -1;
    bufferPosition = 0;
    bufferStart = 0;
    bufferLength = 0;
}

RAMOutputStream::RAMOutputStream(const RAMFilePtr& f) {
    file = f;

    // make sure that we switch to the first needed buffer lazily
    currentBufferIndex = -1;
    bufferPosition = 0;
    bufferStart = 0;
    bufferLength = 0;
}

RAMOutputStream::~RAMOutputStream() {
}

void RAMOutputStream::writeTo(const IndexOutputPtr& out) {
    flush();
    int64_t end = file->length;
    int64_t pos = 0;
    int32_t buffer = 0;
    while (pos < end) {
        int32_t length = BUFFER_SIZE;
        int64_t nextPos = pos + length;
        if (nextPos > end) { // at the last buffer
            length = (int32_t)(end - pos);
        }
        out->writeBytes(file->getBuffer(buffer++).get(), length);
        pos = nextPos;
    }
}

void RAMOutputStream::reset() {
    currentBuffer.reset();
    currentBufferIndex = -1;
    bufferPosition = 0;
    bufferStart = 0;
    bufferLength = 0;
    file->setLength(0);
}

void RAMOutputStream::close() {
    flush();
}

void RAMOutputStream::seek(int64_t pos) {
    // set the file length in case we seek back and flush() has not been called yet
    setFileLength();
    if ((int64_t)pos < bufferStart || (int64_t)pos >= bufferStart + bufferLength) {
        currentBufferIndex = (int32_t)(pos / BUFFER_SIZE);
        switchCurrentBuffer();
    }
    bufferPosition = (int32_t)(pos % BUFFER_SIZE);
}

int64_t RAMOutputStream::length() {
    return file->length;
}

void RAMOutputStream::writeByte(uint8_t b) {
    if (bufferPosition == bufferLength) {
        ++currentBufferIndex;
        switchCurrentBuffer();
    }
    currentBuffer[bufferPosition++] = b;
}

void RAMOutputStream::writeBytes(const uint8_t* b, int32_t offset, int32_t length) {
    while (length > 0) {
        BOOST_ASSERT(b != NULL);
        if (bufferPosition == bufferLength) {
            ++currentBufferIndex;
            switchCurrentBuffer();
        }

        int32_t remainInBuffer = currentBuffer.size() - bufferPosition;
        int32_t bytesToCopy = length < remainInBuffer ? length : remainInBuffer;
        MiscUtils::arrayCopy(b, offset, currentBuffer.get(), bufferPosition, bytesToCopy);
        offset += bytesToCopy;
        length -= bytesToCopy;
        bufferPosition += bytesToCopy;
    }
}

void RAMOutputStream::switchCurrentBuffer() {
    if (currentBufferIndex == file->numBuffers()) {
        currentBuffer = file->addBuffer(BUFFER_SIZE);
    } else {
        currentBuffer = file->getBuffer(currentBufferIndex);
    }
    bufferPosition = 0;
    bufferStart = (int64_t)BUFFER_SIZE * (int64_t)currentBufferIndex;
    bufferLength = currentBuffer.size();
}

void RAMOutputStream::setFileLength() {
    int64_t pointer = bufferStart + bufferPosition;
    if (pointer > file->length) {
        file->setLength(pointer);
    }
}

void RAMOutputStream::flush() {
    file->setLastModified(MiscUtils::currentTimeMillis());
    setFileLength();
}

int64_t RAMOutputStream::getFilePointer() {
    return currentBufferIndex < 0 ? 0 : bufferStart + bufferPosition;
}

int64_t RAMOutputStream::sizeInBytes() {
    return file->numBuffers() * BUFFER_SIZE;
}

}
