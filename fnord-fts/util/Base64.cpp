/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Base64.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"

namespace Lucene {

const String Base64::BASE64_CHARS = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Base64::~Base64() {
}

String Base64::encode(ByteArray bytes) {
    return encode(bytes.get(), bytes.size());
}

String Base64::encode(const uint8_t* bytes, int32_t length) {
    String result;
    uint8_t byteArray3[3];
    uint8_t byteArray4[4];
    int32_t i = 0;

    while (length--) {
        byteArray3[i++] = *(bytes++);
        if (i == 3) {
            byteArray4[0] = (byteArray3[0] & 0xfc) >> 2;
            byteArray4[1] = ((byteArray3[0] & 0x03) << 4) + ((byteArray3[1] & 0xf0) >> 4);
            byteArray4[2] = ((byteArray3[1] & 0x0f) << 2) + ((byteArray3[2] & 0xc0) >> 6);
            byteArray4[3] = byteArray3[2] & 0x3f;

            for (i = 0; i < 4; ++i) {
                result += BASE64_CHARS[byteArray4[i]];
            }
            i = 0;
        }
    }

    if (i != 0) {
        for (int32_t j = i; j < 3; ++j) {
            byteArray3[j] = 0;
        }

        byteArray4[0] = (byteArray3[0] & 0xfc) >> 2;
        byteArray4[1] = ((byteArray3[0] & 0x03) << 4) + ((byteArray3[1] & 0xf0) >> 4);
        byteArray4[2] = ((byteArray3[1] & 0x0f) << 2) + ((byteArray3[2] & 0xc0) >> 6);
        byteArray4[3] = byteArray3[2] & 0x3f;

        for (int32_t j = 0; j < i + 1; ++j) {
            result += BASE64_CHARS[byteArray4[j]];
        }

        while (i++ < 3) {
            result += L'=';
        }
    }
    return result;
}

ByteArray Base64::decode(const String& str) {
    int32_t length = str.length();
    uint8_t byteArray4[4];
    uint8_t byteArray3[3];

    int32_t i = 0;
    int32_t charIndex = 0;

    ByteArray result(ByteArray::newInstance(length / 2));
    int32_t resultIndex = 0;

    while (length-- && str[charIndex] != L'=' && isBase64(str[charIndex])) {
        byteArray4[i++] = (uint8_t)str[charIndex++];
        if (i == 4) {
            for (i = 0; i < 4; ++i) {
                byteArray4[i] = BASE64_CHARS.find(byteArray4[i]);
            }
            byteArray3[0] = (byteArray4[0] << 2) + ((byteArray4[1] & 0x30) >> 4);
            byteArray3[1] = ((byteArray4[1] & 0xf) << 4) + ((byteArray4[2] & 0x3c) >> 2);
            byteArray3[2] = ((byteArray4[2] & 0x3) << 6) + byteArray4[3];

            for (i = 0; i < 3; ++i) {
                if (resultIndex >= result.size()) {
                    result.resize((int32_t)((double)result.size() * 1.5));
                }
                result[resultIndex++] = byteArray3[i];
            }

            i = 0;
        }
    }

    if (i != 0) {
        for (int32_t j = i; j < 4; ++j) {
            byteArray4[j] = 0;
        }
        for (int32_t j = 0; j < 4; ++j) {
            byteArray4[j] = BASE64_CHARS.find(byteArray4[j]);
        }
        byteArray3[0] = (byteArray4[0] << 2) + ((byteArray4[1] & 0x30) >> 4);
        byteArray3[1] = ((byteArray4[1] & 0xf) << 4) + ((byteArray4[2] & 0x3c) >> 2);
        byteArray3[2] = ((byteArray4[2] & 0x3) << 6) + byteArray4[3];

        for (int32_t j = 0; j < i - 1; ++j) {
            if (resultIndex >= result.size()) {
                result.resize((int32_t)((double)result.size() * 1.5));
            }
            result[resultIndex++] = byteArray3[j];
        }
    }

    result.resize(resultIndex);

    return result;
}

bool Base64::isBase64(wchar_t ch) {
    return (UnicodeUtil::isAlnum(ch) || ch == L'+' || ch == L'/');
}

}
