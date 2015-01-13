/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "IndexOutput.h"
#include "IndexInput.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t IndexOutput::COPY_BUFFER_SIZE = 16384;

IndexOutput::~IndexOutput() {
}

void IndexOutput::writeBytes(const uint8_t* b, int32_t length) {
    writeBytes(b, 0, length);
}

void IndexOutput::writeInt(int32_t i) {
    writeByte((uint8_t)(i >> 24));
    writeByte((uint8_t)(i >> 16));
    writeByte((uint8_t)(i >> 8));
    writeByte((uint8_t)i);
}

void IndexOutput::writeVInt(int32_t i) {
    while ((i & ~0x7f) != 0) {
        writeByte((uint8_t)((i & 0x7f) | 0x80));
        i = MiscUtils::unsignedShift(i, 7);
    }
    writeByte((uint8_t)i);
}

void IndexOutput::writeLong(int64_t i) {
    writeInt((int32_t)(i >> 32));
    writeInt((int32_t)i);
}

void IndexOutput::writeVLong(int64_t i) {
    while ((i & ~0x7f) != 0) {
        writeByte((uint8_t)((i & 0x7f) | 0x80));
        i = MiscUtils::unsignedShift(i, (int64_t)7);
    }
    writeByte((uint8_t)i);
}

void IndexOutput::writeString(const String& s) {
    UTF8ResultPtr utf8Result(newLucene<UTF8Result>());
    StringUtils::toUTF8(s.c_str(), s.length(), utf8Result);
    writeVInt(utf8Result->length);
    writeBytes(utf8Result->result.get(), utf8Result->length);
}

void IndexOutput::writeChars(const String& s, int32_t start, int32_t length) {
    int32_t end = start + length;
    for (int32_t i = start; i < end; ++i) {
        int32_t code = (int32_t)s[i];
        if (code >= 0x01 && code <= 0x7f) {
            writeByte((uint8_t)code);
        } else if (((code >= 0x80) && (code <= 0x7ff)) || code == 0) {
            writeByte((uint8_t)(0xc0 | (code >> 6)));
            writeByte((uint8_t)(0x80 | (code & 0x3f)));
        } else {
            writeByte((uint8_t)(0xe0 | MiscUtils::unsignedShift(code, 12)));
            writeByte((uint8_t)(0x80 | ((code >> 6) & 0x3f)));
            writeByte((uint8_t)(0x80 | (code & 0x3f)));
        }
    }
}

void IndexOutput::copyBytes(const IndexInputPtr& input, int64_t numBytes) {
    BOOST_ASSERT(numBytes >= 0);
    int64_t left = numBytes;
    if (!copyBuffer) {
        copyBuffer = ByteArray::newInstance(COPY_BUFFER_SIZE);
    }
    while (left > 0) {
        int32_t toCopy = left > COPY_BUFFER_SIZE ? COPY_BUFFER_SIZE : (int32_t)left;
        input->readBytes(copyBuffer.get(), 0, toCopy);
        writeBytes(copyBuffer.get(), 0, toCopy);
        left -= toCopy;
    }
}

void IndexOutput::setLength(int64_t length) {
}

void IndexOutput::writeStringStringMap(MapStringString map) {
    if (!map) {
        writeInt(0);
    } else {
        writeInt(map.size());
        for (MapStringString::iterator entry = map.begin(); entry != map.end(); ++entry) {
            writeString(entry->first);
            writeString(entry->second);
        }
    }
}

}
