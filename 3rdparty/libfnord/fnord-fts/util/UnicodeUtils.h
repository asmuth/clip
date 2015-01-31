/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef UNICODEUTILS_H
#define UNICODEUTILS_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

class UnicodeUtil {
public:
    virtual ~UnicodeUtil();

public:
    /// Return true if supplied character is alpha-numeric.
    static bool isAlnum(wchar_t c);

    /// Return true if supplied character is alphabetic.
    static bool isAlpha(wchar_t c);

    /// Return true if supplied character is numeric.
    static bool isDigit(wchar_t c);

    /// Return true if supplied character is a space.
    static bool isSpace(wchar_t c);

    /// Return true if supplied character is uppercase.
    static bool isUpper(wchar_t c);

    /// Return true if supplied character is lowercase.
    static bool isLower(wchar_t c);

    /// Return true if supplied character is other type of letter.
    static bool isOther(wchar_t c);

    /// Return true if supplied character is non-spacing.
    static bool isNonSpacing(wchar_t c);

    /// Return uppercase representation of a given character.
    static wchar_t toUpper(wchar_t c);

    /// Return lowercase representation of a given character.
    static wchar_t toLower(wchar_t c);
};

/// Utility class that contains utf8 and unicode translations.
template <typename TYPE>
class TranslationResult : public LuceneObject {
public:
    TranslationResult() {
        result = Array<TYPE>::newInstance(10);
        length = 0;
    }

public:
    Array<TYPE> result;
    int32_t length;

public:
    void setLength(int32_t length) {
        if (!result) {
            result = Array<TYPE>::newInstance((int32_t)(1.5 * (double)length));
        }
        if (result.size() < length) {
            result.resize((int32_t)(1.5 * (double)length));
        }
        this->length = length;
    }

    void copyText(const TranslationResult& other) {
        setLength(other.length);
        MiscUtils::arrayCopy(other.result.get(), 0, result.get(), 0, other.length);
    }

    void copyText(std::shared_ptr< TranslationResult<TYPE> > other) {
        copyText(*other);
    }
};

class UTF8Result : public TranslationResult<uint8_t> {
public:
    virtual ~UTF8Result();
};

class UnicodeResult : public TranslationResult<wchar_t> {
public:
    virtual ~UnicodeResult();
};

}

}
#endif
