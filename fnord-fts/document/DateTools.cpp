/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "DateTools.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

DateTools::DateOrder DateTools::dateOrder = DateTools::DATEORDER_LOCALE;

DateTools::~DateTools() {
}

String DateTools::dateToString(const boost::posix_time::ptime& date, Resolution resolution) {
    return timeToString(MiscUtils::getTimeMillis(date), resolution);
}

String DateTools::timeToString(int64_t time, Resolution resolution) {
    std::string timeString(boost::posix_time::to_iso_string(boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::milliseconds(time))));
    switch (resolution) {
    case RESOLUTION_YEAR:
        return StringUtils::toUnicode(timeString.substr(0, 4).c_str());
    case RESOLUTION_MONTH:
        return StringUtils::toUnicode(timeString.substr(0, 6).c_str());
    case RESOLUTION_DAY:
        return StringUtils::toUnicode(timeString.substr(0, 8).c_str());
    case RESOLUTION_HOUR:
        return StringUtils::toUnicode(std::string(timeString.substr(0, 8) + timeString.substr(9, 2)).c_str());
    case RESOLUTION_MINUTE:
        return StringUtils::toUnicode(std::string(timeString.substr(0, 8) + timeString.substr(9, 4)).c_str());
    case RESOLUTION_SECOND:
        return StringUtils::toUnicode(std::string(timeString.substr(0, 8) + timeString.substr(9, 6)).c_str());
    case RESOLUTION_MILLISECOND: {
        std::string fraction(timeString.length() > 16 ? timeString.substr(16, 3) : "000" );
        return StringUtils::toUnicode(std::string(timeString.substr(0, 8) + timeString.substr(9, 6) + fraction).c_str());
    }
    case RESOLUTION_NULL:
        // silence static analyzers
        break;
    }

    boost::throw_exception(IllegalArgumentException(L"unknown resolution '" + StringUtils::toString(resolution) + L"'"));
    return L"";
}

int64_t DateTools::stringToTime(const String& dateString) {
    return MiscUtils::getTimeMillis(stringToDate(dateString));
}

boost::posix_time::ptime DateTools::stringToDate(const String& dateString) {
    uint16_t year = dateString.length() >= 4 ? (uint16_t)wcstol(dateString.substr(0, 4).c_str(), 0, 10) : 1970;
    uint16_t month = dateString.length() >= 6 ? (uint16_t)wcstol(dateString.substr(4, 2).c_str(), 0, 10) : 1;
    uint16_t day = dateString.length() >= 8 ? (uint16_t)wcstol(dateString.substr(6, 2).c_str(), 0, 10) : 1;
    uint16_t hour = dateString.length() >= 10 ? (uint16_t)wcstol(dateString.substr(8, 2).c_str(), 0, 10) : 0;
    uint16_t minute = dateString.length() >= 12 ? (uint16_t)wcstol(dateString.substr(10, 2).c_str(), 0, 10) : 0;
    uint16_t second = dateString.length() >= 14 ? (uint16_t)wcstol(dateString.substr(12, 2).c_str(), 0, 10) : 0;
    uint16_t millisecond = dateString.length() >= 16 ? (uint16_t)wcstol(dateString.substr(14, 3).c_str(), 0, 10) : 0;
    boost::posix_time::ptime date;
    try {
        date = boost::posix_time::ptime(boost::gregorian::date(year, month, day),
                                        boost::posix_time::hours(hour) +
                                        boost::posix_time::minutes(minute) +
                                        boost::posix_time::seconds(second) +
                                        boost::posix_time::milliseconds(millisecond));
    } catch (...) {
        boost::throw_exception(ParseException(L"Input is not valid date string: " + dateString));
    }
    return date;
}

boost::posix_time::ptime DateTools::round(const boost::posix_time::ptime& date, Resolution resolution) {
    boost::posix_time::ptime roundDate;

    switch (resolution) {
    case RESOLUTION_YEAR:
        return boost::posix_time::ptime(boost::gregorian::date(date.date().year(), 1, 1));
    case RESOLUTION_MONTH:
        return boost::posix_time::ptime(boost::gregorian::date(date.date().year(), date.date().month(), 1));
    case RESOLUTION_DAY:
        return boost::posix_time::ptime(date.date());
    case RESOLUTION_HOUR:
        return boost::posix_time::ptime(date.date(),
                                        boost::posix_time::hours(boost::posix_time::time_duration(date.time_of_day()).hours()));
    case RESOLUTION_MINUTE:
        return boost::posix_time::ptime(date.date(),
                                        boost::posix_time::hours(boost::posix_time::time_duration(date.time_of_day()).hours()) +
                                        boost::posix_time::minutes(boost::posix_time::time_duration(date.time_of_day()).minutes()));
    case RESOLUTION_SECOND:
        return boost::posix_time::ptime(date.date(),
                                        boost::posix_time::hours(boost::posix_time::time_duration(date.time_of_day()).hours()) +
                                        boost::posix_time::minutes(boost::posix_time::time_duration(date.time_of_day()).minutes()) +
                                        boost::posix_time::seconds(boost::posix_time::time_duration(date.time_of_day()).seconds()));
    case RESOLUTION_MILLISECOND:
        return date;
    case RESOLUTION_NULL:
        // silence static analyzers
        break;
    }

    return boost::posix_time::ptime();
}

int64_t DateTools::round(int64_t time, Resolution resolution) {
    return MiscUtils::getTimeMillis(round(boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::milliseconds(time)), resolution));
}

void DateTools::setDateOrder(DateTools::DateOrder order) {
    dateOrder = order;
}

DateTools::DateOrder DateTools::getDateOrder(std::locale locale) {
    if (dateOrder != DATEORDER_LOCALE) {
        return dateOrder;
    }

    std::locale localeDate(std::locale(locale, new boost::gregorian::date_facet("%x")));
    SingleStringStream controlStream;

    controlStream.imbue(localeDate);
    controlStream << boost::gregorian::date(1974, 10, 20); // Oct 20th 1974

    SingleString controlDate(controlStream.str());
    SingleString::size_type year = controlDate.find("74");
    SingleString::size_type month = controlDate.find("10");
    if (month == SingleString::npos) {
        month = controlDate.find("O");    // safety
    }
    SingleString::size_type day = controlDate.find("20");

    if (year < month) {
        return DATEORDER_YMD;
    } else if (month < day) {
        return DATEORDER_MDY;
    } else {
        return DATEORDER_DMY;
    }
}

boost::posix_time::ptime DateTools::parseDate(const String& dateString, std::locale locale) {
    Collection<String> dateTokens(StringUtils::split(dateString, L",-. /"));
    String delimiter(dateTokens.size() == 1 ? L"" : L"/");
    String paddedDate;
    for (Collection<String>::iterator token = dateTokens.begin(); token != dateTokens.end(); ++token) {
        if (token != dateTokens.begin()) {
            paddedDate += delimiter;
        }
        if (token->length() == 1) {
            paddedDate += L"0" + *token;
        } else {
            paddedDate += *token;
        }
    }

    Collection<String> dateFormats(Collection<String>::newInstance());

    switch (getDateOrder(locale)) {
    case DATEORDER_DMY:
        dateFormats.add(L"%d" + delimiter + L"%m" + delimiter + L"%Y");
        dateFormats.add(L"%d" + delimiter + L"%m" + delimiter + L"%y");
        dateFormats.add(L"%d" + delimiter + L"%b" + delimiter + L"%Y");
        dateFormats.add(L"%d" + delimiter + L"%b" + delimiter + L"%y");
        dateFormats.add(L"%d" + delimiter + L"%B" + delimiter + L"%Y");
        dateFormats.add(L"%d" + delimiter + L"%B" + delimiter + L"%y");
        break;
    case DATEORDER_MDY:
        dateFormats.add(L"%m" + delimiter + L"%d" + delimiter + L"%Y");
        dateFormats.add(L"%m" + delimiter + L"%d" + delimiter + L"%y");
        dateFormats.add(L"%b" + delimiter + L"%d" + delimiter + L"%Y");
        dateFormats.add(L"%b" + delimiter + L"%d" + delimiter + L"%y");
        dateFormats.add(L"%B" + delimiter + L"%d" + delimiter + L"%Y");
        dateFormats.add(L"%B" + delimiter + L"%d" + delimiter + L"%y");
        break;
    case DATEORDER_YMD:
        dateFormats.add(L"%Y" + delimiter + L"%m" + delimiter + L"%d");
        dateFormats.add(L"%y" + delimiter + L"%m" + delimiter + L"%d");
        dateFormats.add(L"%Y" + delimiter + L"%b" + delimiter + L"%d");
        dateFormats.add(L"%y" + delimiter + L"%b" + delimiter + L"%d");
        dateFormats.add(L"%Y" + delimiter + L"%B" + delimiter + L"%d");
        dateFormats.add(L"%y" + delimiter + L"%B" + delimiter + L"%d");
        break;
    case DATEORDER_LOCALE:
        // silence static analyzers
        break;
    }

    boost::date_time::format_date_parser<boost::gregorian::date, wchar_t> parser(L"", locale);
    boost::date_time::special_values_parser<boost::gregorian::date, wchar_t> svp;

    for (Collection<String>::iterator dateFormat = dateFormats.begin(); dateFormat != dateFormats.end(); ++dateFormat) {
        try {
            boost::gregorian::date date = parser.parse_date(paddedDate.c_str(), dateFormat->c_str(), svp);
            if (!date.is_not_a_date()) {
                return boost::posix_time::ptime(date);
            }
        } catch (...) {
        }
    }

    boost::throw_exception(ParseException(L"Invalid date '" + dateString + L"'"));
    return boost::posix_time::ptime();
}

}
