/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DateField.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

DateField::~DateField() {
}

int32_t DateField::DATE_LEN() {
    static int32_t _DATE_LEN = 0;
    if (_DATE_LEN == 0) {
        // make date strings long enough to last a millennium
        _DATE_LEN = (int32_t)StringUtils::toString((int64_t)(1000 * 365 * 24) * (int64_t)(60 * 60 * 1000), StringUtils::CHARACTER_MAX_RADIX).length();
    }
    return _DATE_LEN;
}

const String& DateField::MIN_DATE_STRING() {
    static String _MIN_DATE_STRING;
    if (_MIN_DATE_STRING.empty()) {
        _MIN_DATE_STRING = timeToString(0);
    }
    return _MIN_DATE_STRING;
}

const String& DateField::MAX_DATE_STRING() {
    static String _MAX_DATE_STRING;
    if (_MAX_DATE_STRING.empty()) {
        _MAX_DATE_STRING.resize(DATE_LEN());
        std::fill(_MAX_DATE_STRING.begin(), _MAX_DATE_STRING.end(), L'z');
    }
    return _MAX_DATE_STRING;
}

String DateField::dateToString(const boost::posix_time::ptime& date) {
    return timeToString(MiscUtils::getTimeMillis(date));
}

String DateField::timeToString(int64_t time) {
    if (time < 0) {
        boost::throw_exception(RuntimeException(L"time '" + StringUtils::toString(time) + L"' is too early, must be >= 0"));
    }

    String timeString(DATE_LEN(), L'0');
    timeString += StringUtils::toString(time, StringUtils::CHARACTER_MAX_RADIX);

    return timeString.substr(timeString.length() - DATE_LEN(), DATE_LEN());
}

int64_t DateField::stringToTime(const String& s) {
    return StringUtils::toLong(s, StringUtils::CHARACTER_MAX_RADIX);
}

}
