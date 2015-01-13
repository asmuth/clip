/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DefaultSkipListWriter.h"
#include "IndexOutput.h"
#include "MiscUtils.h"

namespace Lucene {

DefaultSkipListWriter::DefaultSkipListWriter(int32_t skipInterval, int32_t numberOfSkipLevels, int32_t docCount, const IndexOutputPtr& freqOutput, const IndexOutputPtr& proxOutput) : MultiLevelSkipListWriter(skipInterval, numberOfSkipLevels, docCount) {
    curDoc = 0;
    curStorePayloads = false;
    curPayloadLength = 0;
    curFreqPointer = 0;
    curProxPointer = 0;

    this->freqOutput = freqOutput;
    this->proxOutput = proxOutput;

    lastSkipDoc = Collection<int32_t>::newInstance(numberOfSkipLevels);
    lastSkipPayloadLength = Collection<int32_t>::newInstance(numberOfSkipLevels);
    lastSkipFreqPointer = Collection<int64_t>::newInstance(numberOfSkipLevels);
    lastSkipProxPointer = Collection<int64_t>::newInstance(numberOfSkipLevels);
}

DefaultSkipListWriter::~DefaultSkipListWriter() {
}

void DefaultSkipListWriter::setFreqOutput(const IndexOutputPtr& freqOutput) {
    this->freqOutput = freqOutput;
}

void DefaultSkipListWriter::setProxOutput(const IndexOutputPtr& proxOutput) {
    this->proxOutput = proxOutput;
}

void DefaultSkipListWriter::setSkipData(int32_t doc, bool storePayloads, int32_t payloadLength) {
    this->curDoc = doc;
    this->curStorePayloads = storePayloads;
    this->curPayloadLength = payloadLength;
    this->curFreqPointer = freqOutput->getFilePointer();
    if (proxOutput) {
        this->curProxPointer = proxOutput->getFilePointer();
    }
}

void DefaultSkipListWriter::resetSkip() {
    MultiLevelSkipListWriter::resetSkip();
    MiscUtils::arrayFill(lastSkipDoc.begin(), 0, lastSkipDoc.size(), 0);
    MiscUtils::arrayFill(lastSkipPayloadLength.begin(), 0, lastSkipPayloadLength.size(), -1); // we don't have to write the first length in the skip list
    MiscUtils::arrayFill(lastSkipFreqPointer.begin(), 0, lastSkipFreqPointer.size(), freqOutput->getFilePointer());
    if (proxOutput) {
        MiscUtils::arrayFill(lastSkipProxPointer.begin(), 0, lastSkipProxPointer.size(), proxOutput->getFilePointer());
    }
}

void DefaultSkipListWriter::writeSkipData(int32_t level, const IndexOutputPtr& skipBuffer) {
    // To efficiently store payloads in the posting lists we do not store the length of
    // every payload. Instead we omit the length for a payload if the previous payload had
    // the same length.
    // However, in order to support skipping the payload length at every skip point must be known.
    // So we use the same length encoding that we use for the posting lists for the skip data as well:
    // Case 1: current field does not store payloads
    //           SkipDatum                 --> DocSkip, FreqSkip, ProxSkip
    //           DocSkip,FreqSkip,ProxSkip --> VInt
    //           DocSkip records the document number before every SkipInterval th  document in TermFreqs.
    //           Document numbers are represented as differences from the previous value in the sequence.
    // Case 2: current field stores payloads
    //           SkipDatum                 --> DocSkip, PayloadLength?, FreqSkip,ProxSkip
    //           DocSkip,FreqSkip,ProxSkip --> VInt
    //           PayloadLength             --> VInt
    //         In this case DocSkip/2 is the difference between
    //         the current and the previous value. If DocSkip
    //         is odd, then a PayloadLength encoded as VInt follows,
    //         if DocSkip is even, then it is assumed that the
    //         current payload length equals the length at the previous
    //         skip point
    if (curStorePayloads) {
        int32_t delta = curDoc - lastSkipDoc[level];
        if (curPayloadLength == lastSkipPayloadLength[level]) {
            // the current payload length equals the length at the previous skip point, so we don't store
            // the length again
            skipBuffer->writeVInt(delta * 2);
        } else {
            // the payload length is different from the previous one. We shift the DocSkip, set the lowest
            // bit and store the current payload length as VInt.
            skipBuffer->writeVInt(delta * 2 + 1);
            skipBuffer->writeVInt(curPayloadLength);
            lastSkipPayloadLength[level] = curPayloadLength;
        }
    } else {
        // current field does not store payloads
        skipBuffer->writeVInt(curDoc - lastSkipDoc[level]);
    }
    skipBuffer->writeVInt((int32_t)(curFreqPointer - lastSkipFreqPointer[level]));
    skipBuffer->writeVInt((int32_t)(curProxPointer - lastSkipProxPointer[level]));

    lastSkipDoc[level] = curDoc;

    lastSkipFreqPointer[level] = curFreqPointer;
    lastSkipProxPointer[level] = curProxPointer;
}

}
