/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "unicode/guniprop.h"

namespace Lucene {

UnicodeUtil::~UnicodeUtil() {
}

bool UnicodeUtil::isAlnum(wchar_t c) {
    return g_unichar_isalnum(c);
}

bool UnicodeUtil::isAlpha(wchar_t c) {
    return g_unichar_isalpha(c);
}

bool UnicodeUtil::isDigit(wchar_t c) {
    return g_unichar_isdigit(c);
}

bool UnicodeUtil::isSpace(wchar_t c) {
    return g_unichar_isspace(c);
}

bool UnicodeUtil::isUpper(wchar_t c) {
    return g_unichar_isupper(c);
}

bool UnicodeUtil::isLower(wchar_t c) {
    return g_unichar_islower(c);
}

bool UnicodeUtil::isOther(wchar_t c) {
    return (g_unichar_type(c) == G_UNICODE_OTHER_LETTER);
}

bool UnicodeUtil::isNonSpacing(wchar_t c) {
    return (g_unichar_type(c) == G_UNICODE_NON_SPACING_MARK);
}

wchar_t UnicodeUtil::toUpper(wchar_t c) {
    return (wchar_t)g_unichar_toupper(c);
}

wchar_t UnicodeUtil::toLower(wchar_t c) {
    return (wchar_t)g_unichar_tolower(c);
}

UTF8Result::~UTF8Result() {
}

UnicodeResult::~UnicodeResult() {
}

}
