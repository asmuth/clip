/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultiLevelSkipListReader.h"
#include "BufferedIndexInput.h"
#include "MiscUtils.h"

namespace Lucene {

MultiLevelSkipListReader::MultiLevelSkipListReader(const IndexInputPtr& skipStream, int32_t maxSkipLevels, int32_t skipInterval) {
    this->numberOfLevelsToBuffer = 1;
    this->numberOfSkipLevels = 0;
    this->docCount = 0;
    this->haveSkipped = false;
    this->lastDoc = 0;
    this->lastChildPointer = 0;

    this->skipStream = Collection<IndexInputPtr>::newInstance(maxSkipLevels);
    this->skipPointer = Collection<int64_t>::newInstance(maxSkipLevels);
    this->childPointer = Collection<int64_t>::newInstance(maxSkipLevels);
    this->numSkipped = Collection<int32_t>::newInstance(maxSkipLevels);
    this->maxNumberOfSkipLevels = maxSkipLevels;
    this->skipInterval = Collection<int32_t>::newInstance(maxSkipLevels);
    this->skipStream[0] = skipStream;
    this->inputIsBuffered = boost::dynamic_pointer_cast<BufferedIndexInput>(skipStream).get() != NULL;
    this->skipInterval[0] = skipInterval;
    this->skipDoc = Collection<int32_t>::newInstance(maxSkipLevels);

    MiscUtils::arrayFill(this->skipPointer.begin(), 0, this->skipPointer.size(), 0);
    MiscUtils::arrayFill(this->childPointer.begin(), 0, this->childPointer.size(), 0);
    MiscUtils::arrayFill(this->numSkipped.begin(), 0, this->numSkipped.size(), 0);
    MiscUtils::arrayFill(this->skipDoc.begin(), 0, this->skipDoc.size(), 0);

    for (int32_t i = 1; i < maxSkipLevels; ++i) {
        // cache skip intervals
        this->skipInterval[i] = this->skipInterval[i - 1] * skipInterval;
    }
}

MultiLevelSkipListReader::~MultiLevelSkipListReader() {
}

int32_t MultiLevelSkipListReader::getDoc() {
    return lastDoc;
}

int32_t MultiLevelSkipListReader::skipTo(int32_t target) {
    if (!haveSkipped) {
        // first time, load skip levels
        loadSkipLevels();
        haveSkipped = true;
    }

    // walk up the levels until highest level is found that has a skip for this target
    int32_t level = 0;
    while (level < numberOfSkipLevels - 1 && target > skipDoc[level + 1]) {
        ++level;
    }

    while (level >= 0) {
        if (target > skipDoc[level]) {
            if (!loadNextSkip(level)) {
                continue;
            }
        } else {
            // no more skips on this level, go down one level
            if (level > 0 && lastChildPointer > skipStream[level - 1]->getFilePointer()) {
                seekChild(level - 1);
            }
            --level;
        }
    }

    return numSkipped[0] - skipInterval[0] - 1;
}

bool MultiLevelSkipListReader::loadNextSkip(int32_t level) {
    // we have to skip, the target document is greater than the current skip list entry
    setLastSkipData(level);

    numSkipped[level] += skipInterval[level];

    if (numSkipped[level] > docCount) {
        // this skip list is exhausted
        skipDoc[level] = INT_MAX;
        if (numberOfSkipLevels > level) {
            numberOfSkipLevels = level;
        }
        return false;
    }

    // read next skip entry
    skipDoc[level] += readSkipData(level, skipStream[level]);

    if (level != 0) {
        // read the child pointer if we are not on the leaf level
        childPointer[level] = skipStream[level]->readVLong() + skipPointer[level - 1];
    }

    return true;
}

void MultiLevelSkipListReader::seekChild(int32_t level) {
    skipStream[level]->seek(lastChildPointer);
    numSkipped[level] = numSkipped[level + 1] - skipInterval[level + 1];
    skipDoc[level] = lastDoc;
    if (level > 0) {
        childPointer[level] = skipStream[level]->readVLong() + skipPointer[level - 1];
    }
}

void MultiLevelSkipListReader::close() {
    for (int32_t i = 1; i < skipStream.size(); ++i) {
        if (skipStream[i]) {
            skipStream[i]->close();
        }
    }
}

void MultiLevelSkipListReader::init(int64_t skipPointer, int32_t df) {
    this->skipPointer[0] = skipPointer;
    this->docCount = df;
    MiscUtils::arrayFill(skipDoc.begin(), 0, skipDoc.size(), 0);
    MiscUtils::arrayFill(numSkipped.begin(), 0, numSkipped.size(), 0);
    MiscUtils::arrayFill(childPointer.begin(), 0, childPointer.size(), 0);

    haveSkipped = false;
    for (int32_t i = 1; i < numberOfSkipLevels; ++i) {
        skipStream[i].reset();
    }
}

void MultiLevelSkipListReader::loadSkipLevels() {
    numberOfSkipLevels = docCount == 0 ? 0 : (int32_t)std::floor(std::log((double)docCount) / std::log((double)skipInterval[0]));
    if (numberOfSkipLevels > maxNumberOfSkipLevels) {
        numberOfSkipLevels = maxNumberOfSkipLevels;
    }

    skipStream[0]->seek(skipPointer[0]);

    int32_t toBuffer = numberOfLevelsToBuffer;

    for (int32_t i = numberOfSkipLevels - 1; i > 0; --i) {
        // the length of the current level
        int64_t length = skipStream[0]->readVLong();

        // the start pointer of the current level
        skipPointer[i] = skipStream[0]->getFilePointer();

        if (toBuffer > 0) {
            // buffer this level
            skipStream[i] = newLucene<SkipBuffer>(skipStream[0], (int32_t)length);
            --toBuffer;
        } else {
            // clone this stream, it is already at the start of the current level
            skipStream[i] = boost::dynamic_pointer_cast<IndexInput>(skipStream[0]->clone());
            if (inputIsBuffered && length < BufferedIndexInput::BUFFER_SIZE) {
                boost::dynamic_pointer_cast<BufferedIndexInput>(skipStream[i])->setBufferSize((int32_t)length);
            }

            // move base stream beyond the current level
            skipStream[0]->seek(skipStream[0]->getFilePointer() + length);
        }
    }

    // use base stream for the lowest level
    skipPointer[0] = skipStream[0]->getFilePointer();
}

void MultiLevelSkipListReader::setLastSkipData(int32_t level) {
    lastDoc = skipDoc[level];
    lastChildPointer = childPointer[level];
}

SkipBuffer::SkipBuffer(const IndexInputPtr& input, int32_t length) {
    pos = 0;
    data = ByteArray::newInstance(length);
    pointer = input->getFilePointer();
    input->readBytes(data.get(), 0, length);
}

SkipBuffer::~SkipBuffer() {
}

void SkipBuffer::close() {
    data.reset();
}

int64_t SkipBuffer::getFilePointer() {
    return (pointer + pos);
}

int64_t SkipBuffer::length() {
    return data.size();
}

uint8_t SkipBuffer::readByte() {
    return data[pos++];
}

void SkipBuffer::readBytes(uint8_t* b, int32_t offset, int32_t length) {
    MiscUtils::arrayCopy(data.get(), pos, b, offset, length);
    pos += length;
}

void SkipBuffer::seek(int64_t pos) {
    this->pos = (int32_t)(pos - pointer);
}

}
