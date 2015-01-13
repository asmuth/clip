/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DefaultSkipListReader.h"
#include "MiscUtils.h"

namespace Lucene {

DefaultSkipListReader::DefaultSkipListReader(const IndexInputPtr& skipStream, int32_t maxSkipLevels, int32_t skipInterval)
    : MultiLevelSkipListReader(skipStream, maxSkipLevels, skipInterval) {
    currentFieldStoresPayloads = false;
    lastFreqPointer = 0;
    lastProxPointer = 0;
    lastPayloadLength = 0;

    freqPointer = Collection<int64_t>::newInstance(maxSkipLevels);
    proxPointer = Collection<int64_t>::newInstance(maxSkipLevels);
    payloadLength = Collection<int32_t>::newInstance(maxSkipLevels);

    MiscUtils::arrayFill(freqPointer.begin(), 0, freqPointer.size(), 0);
    MiscUtils::arrayFill(proxPointer.begin(), 0, proxPointer.size(), 0);
    MiscUtils::arrayFill(payloadLength.begin(), 0, payloadLength.size(), 0);
}

DefaultSkipListReader::~DefaultSkipListReader() {
}

void DefaultSkipListReader::init(int64_t skipPointer, int64_t freqBasePointer, int64_t proxBasePointer, int32_t df, bool storesPayloads) {
    MultiLevelSkipListReader::init(skipPointer, df);
    this->currentFieldStoresPayloads = storesPayloads;
    lastFreqPointer = freqBasePointer;
    lastProxPointer = proxBasePointer;

    MiscUtils::arrayFill(freqPointer.begin(), 0, freqPointer.size(), freqBasePointer);
    MiscUtils::arrayFill(proxPointer.begin(), 0, proxPointer.size(), proxBasePointer);
    MiscUtils::arrayFill(payloadLength.begin(), 0, payloadLength.size(), 0);
}

int64_t DefaultSkipListReader::getFreqPointer() {
    return lastFreqPointer;
}

int64_t DefaultSkipListReader::getProxPointer() {
    return lastProxPointer;
}

int32_t DefaultSkipListReader::getPayloadLength() {
    return lastPayloadLength;
}

void DefaultSkipListReader::seekChild(int32_t level) {
    MultiLevelSkipListReader::seekChild(level);
    freqPointer[level] = lastFreqPointer;
    proxPointer[level] = lastProxPointer;
    payloadLength[level] = lastPayloadLength;
}

void DefaultSkipListReader::setLastSkipData(int32_t level) {
    MultiLevelSkipListReader::setLastSkipData(level);
    lastFreqPointer = freqPointer[level];
    lastProxPointer = proxPointer[level];
    lastPayloadLength = payloadLength[level];
}

int32_t DefaultSkipListReader::readSkipData(int32_t level, const IndexInputPtr& skipStream) {
    int32_t delta;
    if (currentFieldStoresPayloads) {
        // The current field stores payloads.  If the doc delta is odd then we have to read the current
        // payload length because it differs from the length of the previous payload
        delta = skipStream->readVInt();
        if ((delta & 1) != 0) {
            payloadLength[level] = skipStream->readVInt();
        }
        delta = MiscUtils::unsignedShift(delta, 1);
    } else {
        delta = skipStream->readVInt();
    }

    freqPointer[level] += skipStream->readVInt();
    proxPointer[level] += skipStream->readVInt();

    return delta;
}

}
