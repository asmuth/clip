/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermInfosWriter.h"
#include "Directory.h"
#include "IndexOutput.h"
#include "Term.h"
#include "TermInfo.h"
#include "FieldInfos.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// The file format version, a negative number.
const int32_t TermInfosWriter::FORMAT = -3;

/// Changed strings to true utf8 with length-in-bytes not length-in-chars.
const int32_t TermInfosWriter::FORMAT_VERSION_UTF8_LENGTH_IN_BYTES = -4;

/// NOTE: always change this if you switch to a new format.
const int32_t TermInfosWriter::FORMAT_CURRENT = TermInfosWriter::FORMAT_VERSION_UTF8_LENGTH_IN_BYTES;

TermInfosWriter::TermInfosWriter(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval) {
    initialize(directory, segment, fis, interval, false);
    otherWriter = newLucene<TermInfosWriter>(directory, segment, fis, interval, true);
}

TermInfosWriter::TermInfosWriter(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval, bool isIndex) {
    initialize(directory, segment, fis, interval, isIndex);
}

TermInfosWriter::~TermInfosWriter() {
}

void TermInfosWriter::initialize() {
    if (otherWriter) {
        _other = otherWriter;
        otherWriter->_other = shared_from_this();
    }
}

void TermInfosWriter::initialize(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval, bool isi) {
    lastTi = newLucene<TermInfo>();
    utf8Result = newLucene<UTF8Result>();
    lastTermBytes = ByteArray::newInstance(10);
    lastTermBytesLength = 0;
    lastFieldNumber = -1;
    skipInterval = 16;
    maxSkipLevels = 10;
    size = 0;
    lastIndexPointer = 0;

    indexInterval = interval;
    fieldInfos = fis;
    isIndex = isi;
    output = directory->createOutput(segment + (isIndex ? L".tii" : L".tis"));
    output->writeInt(FORMAT_CURRENT); // write format
    output->writeLong(0); // leave space for size
    output->writeInt(indexInterval); // write indexInterval
    output->writeInt(skipInterval); // write skipInterval
    output->writeInt(maxSkipLevels); // write maxSkipLevels
    BOOST_ASSERT(initUnicodeResults());
}

void TermInfosWriter::add(const TermPtr& term, const TermInfoPtr& ti) {
    StringUtils::toUTF8(term->_text.c_str(), term->_text.size(), utf8Result);
    add(fieldInfos->fieldNumber(term->_field), utf8Result->result, utf8Result->length, ti);
}

bool TermInfosWriter::initUnicodeResults() {
    unicodeResult1 = newLucene<UnicodeResult>();
    unicodeResult2 = newLucene<UnicodeResult>();
    return true;
}

int32_t TermInfosWriter::compareToLastTerm(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength) {
    if (lastFieldNumber != fieldNumber) {
        int32_t cmp = fieldInfos->fieldName(lastFieldNumber).compare(fieldInfos->fieldName(fieldNumber));
        // If there is a field named "" (empty string) then we will get 0 on this comparison, yet, it's "OK".
        // But it's not OK if two different field numbers map to the same name.
        if (cmp != 0 || lastFieldNumber != -1) {
            return cmp;
        }
    }

    StringUtils::toUnicode(lastTermBytes.get(), lastTermBytesLength, unicodeResult1);
    StringUtils::toUnicode(termBytes.get(), termBytesLength, unicodeResult2);
    int32_t len = std::min(unicodeResult1->length, unicodeResult2->length);

    for (int32_t i = 0; i < len; ++i) {
        wchar_t ch1 = unicodeResult1->result[i];
        wchar_t ch2 = unicodeResult2->result[i];
        if (ch1 != ch2) {
            return (ch1 - ch2);
        }
    }
    return (unicodeResult1->length - unicodeResult2->length);
}

void TermInfosWriter::add(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength, const TermInfoPtr& ti) {
    // terms out of order?
    BOOST_ASSERT(compareToLastTerm(fieldNumber, termBytes, termBytesLength) < 0 || (isIndex && termBytesLength == 0 && lastTermBytesLength == 0));

    BOOST_ASSERT(ti->freqPointer >= lastTi->freqPointer); // freqPointer out of order?
    BOOST_ASSERT(ti->proxPointer >= lastTi->proxPointer); // proxPointer out of order?

    TermInfosWriterPtr other(_other);

    if (!isIndex && size % indexInterval == 0) {
        other->add(lastFieldNumber, lastTermBytes, lastTermBytesLength, lastTi);    // add an index term
    }

    writeTerm(fieldNumber, termBytes, termBytesLength); // write term

    output->writeVInt(ti->docFreq); // write doc freq
    output->writeVLong(ti->freqPointer - lastTi->freqPointer); // write pointers
    output->writeVLong(ti->proxPointer - lastTi->proxPointer);

    if (ti->docFreq >= skipInterval) {
        output->writeVInt(ti->skipOffset);
    }

    if (isIndex) {
        output->writeVLong(other->output->getFilePointer() - lastIndexPointer);
        lastIndexPointer = other->output->getFilePointer(); // write pointer
    }

    lastFieldNumber = fieldNumber;
    lastTi->set(ti);
    ++size;
}

void TermInfosWriter::writeTerm(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength) {
    // Compute prefix in common with last term
    int32_t start = 0;
    int32_t limit = std::min(termBytesLength, lastTermBytesLength);
    while (start < limit) {
        if (termBytes[start] != lastTermBytes[start]) {
            break;
        }
        ++start;
    }

    int32_t length = termBytesLength - start;
    output->writeVInt(start); // write shared prefix length
    output->writeVInt(length); // write delta length
    output->writeBytes(termBytes.get(), start, length); // write delta bytes
    output->writeVInt(fieldNumber); // write field num
    if (lastTermBytes.size() < termBytesLength) {
        lastTermBytes.resize((int32_t)((double)termBytesLength * 1.5));
    }
    MiscUtils::arrayCopy(termBytes.get(), start, lastTermBytes.get(), start, length);
    lastTermBytesLength = termBytesLength;
}

void TermInfosWriter::close() {
    output->seek(4); // write size after format
    output->writeLong(size);
    output->close();

    if (!isIndex) {
        TermInfosWriterPtr(_other)->close();
    }
}

}
