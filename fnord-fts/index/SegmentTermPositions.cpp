/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentTermPositions.h"
#include "SegmentReader.h"
#include "_SegmentReader.h"
#include "TermInfo.h"
#include "IndexInput.h"
#include "MiscUtils.h"

namespace Lucene {

SegmentTermPositions::SegmentTermPositions(const SegmentReaderPtr& parent) : SegmentTermDocs(parent) {
    this->proxCount = 0;
    this->position = 0;
    this->payloadLength = 0;
    this->needToLoadPayload = false;
    this->lazySkipPointer = -1;
    this->lazySkipProxCount = 0;
}

SegmentTermPositions::~SegmentTermPositions() {
}

void SegmentTermPositions::seek(const TermInfoPtr& ti, const TermPtr& term) {
    SegmentTermDocs::seek(ti, term);
    if (ti) {
        lazySkipPointer = ti->proxPointer;
    }
    lazySkipProxCount = 0;
    proxCount = 0;
    payloadLength = 0;
    needToLoadPayload = false;
}

void SegmentTermPositions::close() {
    SegmentTermDocs::close();
    if (proxStream) {
        proxStream->close();
    }
}

int32_t SegmentTermPositions::nextPosition() {
    if (currentFieldOmitTermFreqAndPositions) {
        // This field does not store term freq, positions, payloads
        return 0;
    }

    // perform lazy skips if necessary
    lazySkip();
    --proxCount;
    position += readDeltaPosition();
    return position;
}

int32_t SegmentTermPositions::readDeltaPosition() {
    int32_t delta = proxStream->readVInt();
    if (currentFieldStoresPayloads) {
        // if the current field stores payloads then the position delta is shifted one bit to the left.
        // if the LSB is set, then we have to read the current payload length
        if ((delta & 1) != 0) {
            payloadLength = proxStream->readVInt();
        }
        delta = MiscUtils::unsignedShift(delta, 1);
        needToLoadPayload = true;
    }
    return delta;
}

void SegmentTermPositions::skippingDoc() {
    // we remember to skip a document lazily
    lazySkipProxCount += _freq;
}

bool SegmentTermPositions::next() {
    // we remember to skip the remaining positions of the current document lazily
    lazySkipProxCount += proxCount;

    if (SegmentTermDocs::next()) {
        proxCount = _freq; // note frequency
        position = 0; // reset position
        return true;
    }
    return false;
}

int32_t SegmentTermPositions::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    boost::throw_exception(UnsupportedOperationException(L"TermPositions does not support processing multiple documents in one call. Use TermDocs instead."));
    return 0;
}

void SegmentTermPositions::skipProx(int64_t proxPointer, int32_t payloadLength) {
    // we save the pointer, we might have to skip there lazily
    lazySkipPointer = proxPointer;
    lazySkipProxCount = 0;
    proxCount = 0;
    this->payloadLength = payloadLength;
    needToLoadPayload = false;
}

void SegmentTermPositions::skipPositions(int32_t n) {
    BOOST_ASSERT(!currentFieldOmitTermFreqAndPositions);
    for (int32_t i = n; i > 0; --i) { // skip unread positions
        readDeltaPosition();
        skipPayload();
    }
}

void SegmentTermPositions::skipPayload() {
    if (needToLoadPayload && payloadLength > 0) {
        proxStream->seek(proxStream->getFilePointer() + payloadLength);
    }
    needToLoadPayload = false;
}

void SegmentTermPositions::lazySkip() {
    if (!proxStream) {
        // clone lazily
        proxStream = boost::dynamic_pointer_cast<IndexInput>(SegmentReaderPtr(_parent)->core->proxStream->clone());
    }

    // we might have to skip the current payload if it was not read yet
    skipPayload();

    if (lazySkipPointer != -1) {
        proxStream->seek(lazySkipPointer);
        lazySkipPointer = -1;
    }

    if (lazySkipProxCount != 0) {
        skipPositions(lazySkipProxCount);
        lazySkipProxCount = 0;
    }
}

int32_t SegmentTermPositions::getPayloadLength() {
    return payloadLength;
}

ByteArray SegmentTermPositions::getPayload(ByteArray data, int32_t offset) {
    if (!needToLoadPayload) {
        boost::throw_exception(IOException(L"Either no payload exists at this term position or an attempt was made to load it more than once."));
    }

    // read payloads lazily
    ByteArray retArray;
    int32_t retOffset = 0;
    if (!data || data.size() - offset < payloadLength) {
        // the array is too small to store the payload data, so we allocate a new one
        retArray = ByteArray::newInstance(payloadLength);
        retOffset = 0;
    } else {
        retArray = data;
        retOffset = offset;
    }
    proxStream->readBytes(retArray.get(), retOffset, payloadLength);
    needToLoadPayload = false;
    return retArray;
}

bool SegmentTermPositions::isPayloadAvailable() {
    return (needToLoadPayload && payloadLength > 0);
}

}
