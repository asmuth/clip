/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentTermEnum.h"
#include "TermInfosWriter.h"
#include "IndexInput.h"
#include "TermBuffer.h"
#include "TermInfo.h"
#include "StringUtils.h"

namespace Lucene {

SegmentTermEnum::SegmentTermEnum() {
    format = 0;
    termBuffer = newLucene<TermBuffer>();
    prevBuffer = newLucene<TermBuffer>();
    scanBuffer = newLucene<TermBuffer>();
    _termInfo = newLucene<TermInfo>();
    formatM1SkipInterval = 0;
    size = 0;
    position = -1;
    indexPointer = 0;
    indexInterval = 0;
    skipInterval = 0;
    maxSkipLevels = 0;

    isIndex = false;
    maxSkipLevels = 0;
}

SegmentTermEnum::SegmentTermEnum(const IndexInputPtr& i, const FieldInfosPtr& fis, bool isi) {
    format = 0;
    termBuffer = newLucene<TermBuffer>();
    prevBuffer = newLucene<TermBuffer>();
    scanBuffer = newLucene<TermBuffer>();
    _termInfo = newLucene<TermInfo>();
    formatM1SkipInterval = 0;
    size = 0;
    position = -1;
    indexPointer = 0;
    indexInterval = 0;
    skipInterval = 0;
    maxSkipLevels = 0;

    input = i;
    fieldInfos = fis;
    isIndex = isi;
    maxSkipLevels = 1; // use single-level skip lists for formats > -3

    int32_t firstInt = input->readInt();
    if (firstInt >= 0) {
        // original-format file, without explicit format version number
        format = 0;
        size = firstInt;

        // back-compatible settings
        indexInterval = 128;
        skipInterval = INT_MAX; // switch off skipTo optimization
    } else {
        // we have a format version number
        format = firstInt;

        // check that it is a format we can understand
        if (format < TermInfosWriter::FORMAT_CURRENT) {
            boost::throw_exception(CorruptIndexException(L"Unknown format version:" + StringUtils::toString(format) + L" expected " + StringUtils::toString(TermInfosWriter::FORMAT_CURRENT) + L" or higher"));
        }

        size = input->readLong(); // read the size

        if (format == -1) {
            if (!isIndex) {
                indexInterval = input->readInt();
                formatM1SkipInterval = input->readInt();
            }
            // switch off skipTo optimization for file format prior to 1.4rc2
            skipInterval = INT_MAX;
        } else {
            indexInterval = input->readInt();
            skipInterval = input->readInt();
            if (format <= TermInfosWriter::FORMAT) {
                // this new format introduces multi-level skipping
                maxSkipLevels = input->readInt();
            }
        }

        BOOST_ASSERT(indexInterval > 0); // must not be negative
        BOOST_ASSERT(skipInterval > 0); // must not be negative
    }
    if (format > TermInfosWriter::FORMAT_VERSION_UTF8_LENGTH_IN_BYTES) {
        termBuffer->setPreUTF8Strings();
        scanBuffer->setPreUTF8Strings();
        prevBuffer->setPreUTF8Strings();
    }
}

SegmentTermEnum::~SegmentTermEnum() {
}

LuceneObjectPtr SegmentTermEnum::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<SegmentTermEnum>();
    SegmentTermEnumPtr cloneEnum(boost::dynamic_pointer_cast<SegmentTermEnum>(TermEnum::clone(clone)));
    cloneEnum->format = format;
    cloneEnum->isIndex = isIndex;
    cloneEnum->formatM1SkipInterval = formatM1SkipInterval;
    cloneEnum->fieldInfos = fieldInfos;
    cloneEnum->size = size;
    cloneEnum->position = position;
    cloneEnum->indexPointer = indexPointer;
    cloneEnum->indexInterval = indexInterval;
    cloneEnum->skipInterval = skipInterval;
    cloneEnum->maxSkipLevels = maxSkipLevels;

    cloneEnum->input = boost::dynamic_pointer_cast<IndexInput>(input->clone());
    cloneEnum->_termInfo = newLucene<TermInfo>(_termInfo);

    cloneEnum->termBuffer = boost::dynamic_pointer_cast<TermBuffer>(termBuffer->clone());
    cloneEnum->prevBuffer = boost::dynamic_pointer_cast<TermBuffer>(prevBuffer->clone());
    cloneEnum->scanBuffer = newLucene<TermBuffer>();

    return cloneEnum;
}

void SegmentTermEnum::seek(int64_t pointer, int64_t p, const TermPtr& t, const TermInfoPtr& ti) {
    input->seek(pointer);
    position = p;
    termBuffer->set(t);
    prevBuffer->reset();
    _termInfo->set(ti);
}

bool SegmentTermEnum::next() {
    if (position++ >= size - 1) {
        prevBuffer->set(termBuffer);
        termBuffer->reset();
        return false;
    }

    prevBuffer->set(termBuffer);
    termBuffer->read(input, fieldInfos);

    _termInfo->docFreq = input->readVInt(); // read doc freq
    _termInfo->freqPointer += input->readVLong(); // read freq pointer
    _termInfo->proxPointer += input->readVLong(); // read prox pointer

    if (format == -1) {
        // just read skipOffset in order to increment file pointer; value is never used
        // since skipTo is switched off
        if (!isIndex && _termInfo->docFreq > formatM1SkipInterval) {
            _termInfo->skipOffset = input->readVInt();
        }
    } else if (_termInfo->docFreq >= skipInterval) {
        _termInfo->skipOffset = input->readVInt();
    }

    if (isIndex) {
        indexPointer += input->readVLong();    // read index pointer
    }

    return true;
}

int32_t SegmentTermEnum::scanTo(const TermPtr& term) {
    scanBuffer->set(term);
    int32_t count = 0;
    while (scanBuffer->compareTo(termBuffer) > 0 && next()) {
        ++count;
    }
    return count;
}

TermPtr SegmentTermEnum::term() {
    return termBuffer->toTerm();
}

TermPtr SegmentTermEnum::prev() {
    return prevBuffer->toTerm();
}

TermInfoPtr SegmentTermEnum::termInfo() {
    return newLucene<TermInfo>(_termInfo);
}

void SegmentTermEnum::termInfo(const TermInfoPtr& ti) {
    ti->set(_termInfo);
}

int32_t SegmentTermEnum::docFreq() {
    return _termInfo->docFreq;
}

int64_t SegmentTermEnum::freqPointer() {
    return _termInfo->freqPointer;
}

int64_t SegmentTermEnum::proxPointer() {
    return _termInfo->proxPointer;
}

void SegmentTermEnum::close() {
    input->close();
}

}
