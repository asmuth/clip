/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultiLevelSkipListWriter.h"
#include "RAMOutputStream.h"

namespace Lucene {

MultiLevelSkipListWriter::MultiLevelSkipListWriter(int32_t skipInterval, int32_t maxSkipLevels, int32_t df) {
    this->skipInterval = skipInterval;

    // calculate the maximum number of skip levels for this document frequency
    numberOfSkipLevels = df == 0 ? 0 : (int32_t)std::floor(std::log((double)df) / std::log((double)skipInterval));

    // make sure it does not exceed maxSkipLevels
    numberOfSkipLevels = std::max(numberOfSkipLevels, maxSkipLevels);
}

MultiLevelSkipListWriter::~MultiLevelSkipListWriter() {
}

void MultiLevelSkipListWriter::init() {
    skipBuffer = Collection<RAMOutputStreamPtr>::newInstance(numberOfSkipLevels);
    for (int32_t i = 0; i < numberOfSkipLevels; ++i) {
        skipBuffer[i] = newLucene<RAMOutputStream>();
    }
}

void MultiLevelSkipListWriter::resetSkip() {
    // creates new buffers or empties the existing ones
    if (!skipBuffer) {
        init();
    } else {
        for (Collection<RAMOutputStreamPtr>::iterator buffer = skipBuffer.begin(); buffer != skipBuffer.end(); ++buffer) {
            (*buffer)->reset();
        }
    }
}

void MultiLevelSkipListWriter::bufferSkip(int32_t df) {
    int32_t numLevels = 0;

    // determine max level
    for (; (df % skipInterval) == 0 && numLevels < numberOfSkipLevels; df /= skipInterval) {
        ++numLevels;
    }

    int64_t childPointer = 0;

    for (int32_t level = 0; level < numLevels; ++level) {
        writeSkipData(level, skipBuffer[level]);

        int64_t newChildPointer = skipBuffer[level]->getFilePointer();

        if (level != 0) {
            // store child pointers for all levels except the lowest
            skipBuffer[level]->writeVLong(childPointer);
        }

        // remember the childPointer for the next level
        childPointer = newChildPointer;
    }
}

int64_t MultiLevelSkipListWriter::writeSkip(const IndexOutputPtr& output) {
    int64_t skipPointer = output->getFilePointer();
    if (!skipBuffer || skipBuffer.empty()) {
        return skipPointer;
    }

    for (int32_t level = numberOfSkipLevels - 1; level > 0; --level) {
        int64_t length = skipBuffer[level]->getFilePointer();
        if (length > 0) {
            output->writeVLong(length);
            skipBuffer[level]->writeTo(output);
        }
    }
    skipBuffer[0]->writeTo(output);
    return skipPointer;
}


}
