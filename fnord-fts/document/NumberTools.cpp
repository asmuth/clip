/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NumberTools.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t NumberTools::RADIX = 36;
const wchar_t NumberTools::NEGATIVE_PREFIX = L'-';
const wchar_t NumberTools::POSITIVE_PREFIX = L'0';

NumberTools::~NumberTools() {
}

const String& NumberTools::MIN_STRING_VALUE() {
    static String _MIN_STRING_VALUE;
    if (_MIN_STRING_VALUE.empty()) {
        _MIN_STRING_VALUE += NEGATIVE_PREFIX;
        _MIN_STRING_VALUE += L"0000000000000";
    }
    return _MIN_STRING_VALUE;
}

const String& NumberTools::MAX_STRING_VALUE() {
    static String _MAX_STRING_VALUE;
    if (_MAX_STRING_VALUE.empty()) {
        _MAX_STRING_VALUE += POSITIVE_PREFIX;
        _MAX_STRING_VALUE += L"1y2p0ij32e8e7";
    }
    return _MAX_STRING_VALUE;
}

int32_t NumberTools::STR_SIZE() {
    static int32_t _STR_SIZE = 0;
    if (_STR_SIZE == 0) {
        _STR_SIZE = (int32_t)MIN_STRING_VALUE().length();
    }
    return _STR_SIZE;
}

String NumberTools::longToString(int64_t l) {
    if (l == std::numeric_limits<int64_t>::min()) {
        // special case, because long is not symmetric around zero
        return MIN_STRING_VALUE();
    }

    String buf;
    buf.reserve(STR_SIZE());

    if (l < 0) {
        buf += NEGATIVE_PREFIX;
        l = std::numeric_limits<int64_t>::max() + l + 1;
    }
    buf += POSITIVE_PREFIX;

    String num(StringUtils::toString(l, RADIX));

    int32_t padLen = (int32_t)(STR_SIZE() - num.length() - buf.length());
    while (padLen-- > 0) {
        buf += L'0';
    }

    return buf + num;
}

int64_t NumberTools::stringToLong(const String& str) {
    if ((int32_t)str.length() != STR_SIZE()) {
        boost::throw_exception(NumberFormatException(L"string is the wrong size"));
    }

    if (str == MIN_STRING_VALUE()) {
        return std::numeric_limits<int64_t>::min();
    }

    wchar_t prefix = str[0];
    int64_t l = StringUtils::toLong(str.substr(1), RADIX);

    if (prefix == POSITIVE_PREFIX) {
        // nop
    } else if (prefix == NEGATIVE_PREFIX) {
        l = l - std::numeric_limits<int64_t>::max() - 1;
    } else {
        boost::throw_exception(NumberFormatException(L"string does not begin with the correct prefix"));
    }

    return l;
}

}
