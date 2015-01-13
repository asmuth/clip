/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentTermDocs.h"
#include "SegmentReader.h"
#include "_SegmentReader.h"
#include "SegmentTermEnum.h"
#include "IndexInput.h"
#include "TermInfosReader.h"
#include "FieldInfos.h"
#include "FieldInfo.h"
#include "Term.h"
#include "TermInfo.h"
#include "DefaultSkipListReader.h"
#include "BitVector.h"
#include "MiscUtils.h"

namespace Lucene {

SegmentTermDocs::SegmentTermDocs(const SegmentReaderPtr& parent) {
    this->_parent = parent;
    this->count = 0;
    this->df = 0;
    this->_doc = 0;
    this->_freq = 0;
    this->freqBasePointer = 0;
    this->proxBasePointer = 0;
    this->skipPointer = 0;
    this->haveSkipped = false;
    this->currentFieldStoresPayloads = false;
    this->currentFieldOmitTermFreqAndPositions = false;

    this->_freqStream = boost::dynamic_pointer_cast<IndexInput>(parent->core->freqStream->clone());
    {
        SyncLock parentLock(parent);
        this->deletedDocs = parent->deletedDocs;
    }
    this->skipInterval = parent->core->getTermsReader()->getSkipInterval();
    this->maxSkipLevels = parent->core->getTermsReader()->getMaxSkipLevels();
}

SegmentTermDocs::~SegmentTermDocs() {
}

void SegmentTermDocs::seek(const TermPtr& term) {
    TermInfoPtr ti(SegmentReaderPtr(_parent)->core->getTermsReader()->get(term));
    seek(ti, term);
}

void SegmentTermDocs::seek(const TermEnumPtr& termEnum) {
    TermInfoPtr ti;
    TermPtr term;

    SegmentTermEnumPtr segmentTermEnum(boost::dynamic_pointer_cast<SegmentTermEnum>(termEnum));
    SegmentReaderPtr parent(_parent);

    // use comparison of fieldinfos to verify that termEnum belongs to the same segment as this SegmentTermDocs
    if (segmentTermEnum && segmentTermEnum->fieldInfos == parent->core->fieldInfos) { // optimized case
        term = segmentTermEnum->term();
        ti = segmentTermEnum->termInfo();
    } else { // punt case
        term = termEnum->term();
        ti = parent->core->getTermsReader()->get(term);
    }

    seek(ti, term);
}

void SegmentTermDocs::seek(const TermInfoPtr& ti, const TermPtr& term) {
    count = 0;
    FieldInfoPtr fi(SegmentReaderPtr(_parent)->core->fieldInfos->fieldInfo(term->_field));
    currentFieldOmitTermFreqAndPositions = fi ? fi->omitTermFreqAndPositions : false;
    currentFieldStoresPayloads = fi ? fi->storePayloads : false;
    if (!ti) {
        df = 0;
    } else {
        df = ti->docFreq;
        _doc = 0;
        freqBasePointer = ti->freqPointer;
        proxBasePointer = ti->proxPointer;
        skipPointer = freqBasePointer + ti->skipOffset;
        _freqStream->seek(freqBasePointer);
        haveSkipped = false;
    }
}

void SegmentTermDocs::close() {
    _freqStream->close();
    if (skipListReader) {
        skipListReader->close();
    }
}

int32_t SegmentTermDocs::doc() {
    return _doc;
}

int32_t SegmentTermDocs::freq() {
    return _freq;
}

void SegmentTermDocs::skippingDoc() {
}

bool SegmentTermDocs::next() {
    while (true) {
        if (count == df) {
            return false;
        }
        int32_t docCode = _freqStream->readVInt();

        if (currentFieldOmitTermFreqAndPositions) {
            _doc += docCode;
            _freq = 1;
        } else {
            _doc += MiscUtils::unsignedShift(docCode, 1); // shift off low bit
            if ((docCode & 1) != 0) { // if low bit is set
                _freq = 1;    // freq is one
            } else {
                _freq = _freqStream->readVInt();    // else read freq
            }
        }

        ++count;

        if (!deletedDocs || !deletedDocs->get(_doc)) {
            break;
        }
        skippingDoc();
    }
    return true;
}

int32_t SegmentTermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    int32_t length = docs.size();
    if (currentFieldOmitTermFreqAndPositions) {
        return readNoTf(docs, freqs, length);
    } else {
        int32_t i = 0;
        while (i < length && count < df) {
            // manually inlined call to next() for speed
            int32_t docCode = _freqStream->readVInt();
            _doc += MiscUtils::unsignedShift(docCode, 1); // shift off low bit
            if ((docCode & 1) != 0) { // if low bit is set
                _freq = 1;    // freq is one
            } else {
                _freq = _freqStream->readVInt();    // else read freq
            }
            ++count;

            if (!deletedDocs || !deletedDocs->get(_doc)) {
                docs[i] = _doc;
                freqs[i] = _freq;
                ++i;
            }
        }
        return i;
    }
}

int32_t SegmentTermDocs::readNoTf(Collection<int32_t> docs, Collection<int32_t> freqs, int32_t length) {
    int32_t i = 0;
    while (i < length && count < df) {
        // manually inlined call to next() for speed
        _doc += _freqStream->readVInt();
        ++count;

        if (!deletedDocs || !deletedDocs->get(_doc)) {
            docs[i] = _doc;

            // Hardware freq to 1 when term freqs were not stored in the index
            freqs[i] = 1;
            ++i;
        }
    }
    return i;
}

void SegmentTermDocs::skipProx(int64_t proxPointer, int32_t payloadLength) {
}

bool SegmentTermDocs::skipTo(int32_t target) {
    if (df >= skipInterval) { // optimized case
        if (!skipListReader) {
            skipListReader = newLucene<DefaultSkipListReader>(boost::dynamic_pointer_cast<IndexInput>(_freqStream->clone()), maxSkipLevels, skipInterval);    // lazily clone
        }

        if (!haveSkipped) { // lazily initialize skip stream
            skipListReader->init(skipPointer, freqBasePointer, proxBasePointer, df, currentFieldStoresPayloads);
            haveSkipped = true;
        }

        int32_t newCount = skipListReader->skipTo(target);
        if (newCount > count) {
            _freqStream->seek(skipListReader->getFreqPointer());
            skipProx(skipListReader->getProxPointer(), skipListReader->getPayloadLength());

            _doc = skipListReader->getDoc();
            count = newCount;
        }
    }

    // done skipping, now just scan
    do {
        if (!next()) {
            return false;
        }
    } while (target > _doc);
    return true;
}

IndexInputPtr SegmentTermDocs::freqStream() {
    return _freqStream;
}

void SegmentTermDocs::freqStream(const IndexInputPtr& freqStream) {
    _freqStream = freqStream;
}

}
