/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "fnord-fts/fts.h"

namespace Lucene {

class StringUtils {
public:
    /// Maximum length of UTF encoding.
    static const int32_t MAX_ENCODING_UTF8_SIZE;

    /// Default character radix.
    static const int32_t CHARACTER_MAX_RADIX;

public:
    /// Convert uft8 buffer into unicode.
    static int32_t toUnicode(const uint8_t* utf8, int32_t length, CharArray unicode);

    /// Convert uft8 buffer into unicode.
    static int32_t toUnicode(const uint8_t* utf8, int32_t length, const UnicodeResultPtr& unicodeResult);

    /// Convert uft8 buffer into unicode.
    static String toUnicode(const uint8_t* utf8, int32_t length);

    /// Convert uft8 string into unicode.
    static String toUnicode(const SingleString& s);

    /// Convert unicode buffer into uft8.
    static int32_t toUTF8(const wchar_t* unicode, int32_t length, ByteArray utf8);

    /// Convert unicode buffer into uft8.
    static int32_t toUTF8(const wchar_t* unicode, int32_t length, const UTF8ResultPtr& utf8Result);

    /// Convert unicode buffer into uft8.
    static SingleString toUTF8(const wchar_t* unicode, int32_t length);

    /// Convert unicode string into uft8.
    static SingleString toUTF8(const String& s);

    /// Convert given string to lower case using current locale
    static void toLower(String& str);

    /// Convert given string to lower case using current locale
    static String toLower(const String& str);

    /// Convert given string to upper case using current locale
    static void toUpper(String& str);

    /// Convert given string to upper case using current locale
    static String toUpper(const String& str);

    /// Compare two strings ignoring case differences
    static int32_t compareCase(const String& first, const String& second);

    /// Splits string using given delimiters
    static Collection<String> split(const String& str, const String& delim);

    /// Convert the given string to int32_t.
    static int32_t toInt(const String& value);

    /// Convert the given string to int64_t.
    static int64_t toLong(const String& value);

    /// Return given value as a long integer using base unit.
    static int64_t toLong(const String& value, int32_t base);

    /// Convert the given string to double.
    static double toDouble(const String& value);

    /// Compute the hash code from string.
    static int32_t hashCode(const String& value);

    /// Return given value as a string using base unit.
    static String toString(int64_t value, int32_t base);

    /// Convert any given type to a {@link String}.
    template <class TYPE>
    static String toString(const TYPE& value) {
        StringStream os;
        os << value;
        return os.str();
    }
};

#define UTF8_TO_STRING(utf8) StringUtils::toUnicode(utf8, SIZEOF_ARRAY(utf8))
}

#endif
