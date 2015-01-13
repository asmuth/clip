/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "StringUtils.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "UTF8Stream.h"
#include "Reader.h"
#include "CharFolder.h"

namespace Lucene {

/// Maximum length of UTF encoding.
const int32_t StringUtils::MAX_ENCODING_UTF8_SIZE = 4;

/// Default character radix.
const int32_t StringUtils::CHARACTER_MAX_RADIX = 36;

int32_t StringUtils::toUnicode(const uint8_t* utf8, int32_t length, CharArray unicode) {
    if (length == 0) {
        return 0;
    }
    UTF8Decoder utf8Decoder(utf8, utf8 + length);
    int32_t decodeLength = utf8Decoder.decode(unicode.get(), unicode.size());
    return decodeLength == Reader::READER_EOF ? 0 : decodeLength;
}

int32_t StringUtils::toUnicode(const uint8_t* utf8, int32_t length, const UnicodeResultPtr& unicodeResult) {
    if (length == 0) {
        unicodeResult->length = 0;
    } else {
        if (length > unicodeResult->result.size()) {
            unicodeResult->result.resize(length);
        }
        unicodeResult->length = toUnicode(utf8, length, unicodeResult->result);
    }
    return unicodeResult->length;
}

String StringUtils::toUnicode(const uint8_t* utf8, int32_t length) {
    if (length == 0) {
        return L"";
    }
    CharArray unicode(CharArray::newInstance(length));
    int32_t result = toUnicode(utf8, length, unicode);
    return String(unicode.get(), result);
}

String StringUtils::toUnicode(const SingleString& s) {
    return s.empty() ? L"" : toUnicode((uint8_t*)s.c_str(), s.length());
}

int32_t StringUtils::toUTF8(const wchar_t* unicode, int32_t length, ByteArray utf8) {
    if (length == 0) {
        return 0;
    }
    UTF8Encoder utf8Encoder(unicode, unicode + length);
    int32_t encodeLength = utf8Encoder.encode(utf8.get(), utf8.size());
    return encodeLength == Reader::READER_EOF ? 0 : encodeLength;
}

int32_t StringUtils::toUTF8(const wchar_t* unicode, int32_t length, const UTF8ResultPtr& utf8Result) {
    if (length == 0) {
        utf8Result->length = 0;
    } else {
        if (length * MAX_ENCODING_UTF8_SIZE > utf8Result->result.size()) {
            utf8Result->result.resize(length * MAX_ENCODING_UTF8_SIZE);
        }
        utf8Result->length = toUTF8(unicode, length, utf8Result->result);
    }
    return utf8Result->length;
}

SingleString StringUtils::toUTF8(const wchar_t* unicode, int32_t length) {
    if (length == 0) {
        return "";
    }
    ByteArray utf8(ByteArray::newInstance(length * MAX_ENCODING_UTF8_SIZE));
    int32_t result = toUTF8(unicode, length, utf8);
    return SingleString((char*)utf8.get(), result);
}

SingleString StringUtils::toUTF8(const String& s) {
    return s.empty() ? "" : toUTF8(s.c_str(), s.size());
}

void StringUtils::toLower(String& str) {
    CharFolder::toLower(str.begin(), str.end());
}

String StringUtils::toLower(const String& str) {
    String lowerStr(str);
    toLower(lowerStr);
    return lowerStr;
}

void StringUtils::toUpper(String& str) {
    CharFolder::toUpper(str.begin(), str.end());
}

String StringUtils::toUpper(const String& str) {
    String upperStr(str);
    toUpper(upperStr);
    return upperStr;
}

int32_t StringUtils::compareCase(const String& first, const String& second) {
    return (toLower(first) == toLower(second));
}

Collection<String> StringUtils::split(const String& str, const String& delim) {
    std::vector<String> tokens;
    boost::split(tokens, str, boost::is_any_of(delim.c_str()));
    return Collection<String>::newInstance(tokens.begin(), tokens.end());
}

int32_t StringUtils::toInt(const String& value) {
    if (value.empty()) {
        boost::throw_exception(NumberFormatException());
    }
    if (value.size() > 1 && value[0] == L'-' && !UnicodeUtil::isDigit(value[1])) {
        boost::throw_exception(NumberFormatException());
    }
    if (value[0] != L'-' && !UnicodeUtil::isDigit(value[0])) {
        boost::throw_exception(NumberFormatException());
    }
    return (int32_t)std::wcstol(value.c_str(), NULL, 10);
}

int64_t StringUtils::toLong(const String& value) {
    if (value.empty()) {
        boost::throw_exception(NumberFormatException());
    }
    if (value.size() > 1 && value[0] == L'-' && !UnicodeUtil::isDigit(value[1])) {
        boost::throw_exception(NumberFormatException());
    }
    if (value[0] != L'-' && !UnicodeUtil::isDigit(value[0])) {
        boost::throw_exception(NumberFormatException());
    }
#if defined(_WIN32) || defined(_WIN64)
    return _wcstoi64(value.c_str(), 0, 10);
#else
    return wcstoll(value.c_str(), 0, 10);
#endif
}

int64_t StringUtils::toLong(const String& value, int32_t base) {
    int64_t longValue = 0;
    for (String::const_iterator ptr = value.begin(); ptr != value.end(); ++ptr) {
        longValue = UnicodeUtil::isDigit(*ptr) ? (base * longValue) + (*ptr - L'0') : (base * longValue) + (*ptr - L'a' + 10);
    }
    return longValue;
}

double StringUtils::toDouble(const String& value) {
    if (value.empty()) {
        boost::throw_exception(NumberFormatException());
    }
    if (value.length() > 1 && (value[0] == L'-' || value[0] == L'.') && !UnicodeUtil::isDigit(value[1])) {
        boost::throw_exception(NumberFormatException());
    }
    if (value[0] != L'-' && value[0] != L'.' && !UnicodeUtil::isDigit(value[0])) {
        boost::throw_exception(NumberFormatException());
    }
    return std::wcstod(value.c_str(), NULL);
}

int32_t StringUtils::hashCode(const String& value) {
    int32_t hashCode = 0;
    for (String::const_iterator ptr = value.begin(); ptr != value.end(); ++ptr) {
        hashCode = hashCode * 31 + *ptr;
    }
    return hashCode;
}

String StringUtils::toString(int64_t value, int32_t base) {
    static const wchar_t* digits = L"0123456789abcdefghijklmnopqrstuvwxyz";

    int32_t bufferSize = (sizeof(int32_t) << 3) + 1;
    CharArray baseOutput(CharArray::newInstance(bufferSize));

    wchar_t* ptr = baseOutput.get() + bufferSize - 1;
    *ptr = L'\0';

    do {
        *--ptr = digits[value % base];
        value /= base;
    } while (ptr > baseOutput.get() && value > 0);

    return String(ptr, (baseOutput.get() + bufferSize - 1) - ptr);
}

}

