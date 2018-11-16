/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *   - Laura Schlimmer <laura@eventql.io>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ISO8601.h"
#include "time_constants.h"

namespace {

bool parseTimeOffset(
    const char* begin,
    const char* end,
    CivilTime* date) {
  int32_t sign;
  int32_t offset_sec = 0;

  if (begin + 1 == end && (*begin == 'z' || *begin == 'Z')) {
    date->setOffset(0);
    return true;
  }

  if (*begin == '+') {
    sign = 1;
  } else if (*begin == '-') {
    sign = -1;
  } else {
    return false;
  }

  auto hour_begin = begin + 1;
  auto hour_end = hour_begin + 2;

  if (hour_end > end || !StringUtil::isDigitString(hour_begin, hour_end)) {
    return false;
  }

  try {
    int hour = std::stoi(String(hour_begin, hour_end));
    if (hour >= 0 && hour <= 24) {
      offset_sec += hour * kSecondsPerHour;
    } else {
      return false;
    }
  } catch (...) {
    return false;
  }

  auto minute_begin = hour_end + 1;
  auto minute_end = minute_begin + 2;

  if (minute_end > end ||
      *hour_end != ':' ||
      !StringUtil::isDigitString(minute_begin, minute_end)) {
        return false;
  }

  try {
    int minute = std::stoi(String(minute_begin, minute_end));
    if (minute >= 0 && minute <= 59) {
      offset_sec += minute * kSecondsPerMinute;
    } else {
      return false;
    }
  } catch (...) {
    return false;
  }

  date->setOffset(sign * offset_sec);
  return true;
}

bool parseSecondFrac(
    const char* begin,
    const char* end,
    CivilTime* date) {
  auto frac_begin = begin + 1;
  if (frac_begin > end || *begin != '.') {
    return false;
  }

  auto frac_len = 0;
  while (frac_begin + frac_len <= end && isdigit(*(frac_begin + frac_len))) {
    ++frac_len;
  }

  if (frac_len < 1 || frac_len > 3) {
    return false;
  }

  try {
    auto millis = std::stoul(String(frac_begin, frac_begin + frac_len));
    millis *= pow(10, 3 - frac_len);
    date->setMillisecond(millis);
  } catch (const std::exception&) {
    return false;
  }

  return parseTimeOffset(frac_begin + frac_len, end, date);
}

bool parseSecond(const char* begin, const char* end, CivilTime* date) {
  auto second_begin = begin + 1;
  auto second_end = second_begin + 2;

  if (second_begin > end ||
      *begin != ':' ||
      !StringUtil::isDigitString(second_begin, second_end)) {
        return false;
  }

  try {
    int second = std::stoi(String(second_begin, second_end));
    if (second >= 0 && second <= 59) {
      date->setSecond(second);
    } else {
      return false;
    }
  } catch (const std::exception&) {
    return false;
  }

  return
      parseSecondFrac(second_end, end, date) ||
      parseTimeOffset(second_end, end, date);
}

bool parseMinute(const char* begin, const char* end, CivilTime* date) {
  auto minute_begin = begin + 1;
  auto minute_end = minute_begin + 2;

  if (minute_begin > end ||
      *begin != ':' ||
      !StringUtil::isDigitString(minute_begin, minute_end)) {
        return false;
  }

  try {
    int minute = std::stoi(String(minute_begin, minute_end));
    if (minute >= 0 && minute <= 59) {
      date->setMinute(minute);
    } else {
      return false;
    }
  } catch (const std::exception&) {
    return false;
  }

  return parseSecond(minute_end, end, date);
}


bool parseHour(const char* begin, const char* end, CivilTime* date) {
  auto hour_begin = begin + 1;
  auto hour_end = hour_begin + 2;

  if (hour_begin > end ||
      (*begin != 'T' && *begin != 't') ||
      !StringUtil::isDigitString(hour_begin, hour_end)) {
        return false;
  }

  try {
    int hour = std::stoi(String(hour_begin, hour_end));
    if (hour >= 0 && hour <= 24) {
      date->setHour(hour);
    } else {
      return false;
    }
  } catch (const std::exception&) {
    return false;
  }


  return parseMinute(hour_end, end, date);
}

bool parseDay(const char* begin, const char* end, CivilTime* date) {
  auto day_begin = begin + 1;
  auto day_end = day_begin + 2;

  if (day_begin > end ||
      *begin != '-' ||
      !StringUtil::isDigitString(String(day_begin, day_end))) {
        return false;
  }

  try {
    int day = std::stoi(String(day_begin, day_end));
    //fixme better check
    if (day > 0 && day <= ISO8601::daysInMonth(date->year(), day)) {
      date->setDay(day);
    } else {
      return false;
    }
  } catch (const std::exception&) {
    return false;
  }

  if (day_end < end) {
    return parseHour(day_end, end, date);
  }

  return true;
}

bool parseMonth(const char* begin, const char* end, CivilTime* date) {
  auto month_begin = begin + 1;
  auto month_end = month_begin + 2;

  if (month_end > end ||
      *begin != '-' ||
      !StringUtil::isDigitString(String(month_begin, month_end))) {
        return false;
  }

  try {
    int month = std::stoi(String(month_begin, month_end));
    if (month > 0 && month < 13) {
      date->setMonth(month);
    } else {
      return false;
    }
  } catch (const std::exception&) {
    return false;
  }

  if (month_end < end) {
    return parseDay(month_end, end, date);
  }

  return true;
}

static bool parseYear(const char* begin, const char* end, CivilTime* date) {
  auto year_begin = begin;
  auto year_end = begin + 4;

  if (year_end > end ||
      !StringUtil::isDigitString(String(year_begin, year_end))) {
    return false;
  }


  try {
    date->setYear(std::stoi(String(year_begin, year_end)));
  } catch (const std::exception&) {
    return false;
  }

  if (year_end < end) {
    return parseMonth(year_end, end, date);
  }

  return true;
}

}

Option<CivilTime> ISO8601::parse(const String& str) {
  CivilTime date(nullptr);

  if (!parseYear(str.c_str(), str.c_str() + str.size(), &date)) {
    return None<CivilTime>();
  }

  return Some(date);
}

bool ISO8601::isLeapYear(uint16_t year) {
  if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
    return true;
  } else {
    return false;
  }
}

uint8_t ISO8601::daysInMonth(uint16_t year, uint8_t month) {
  if (month == 2) {
    return (28 + (isLeapYear(year) ? 1 : 0));
  }

  return (31 - (month - 1) % 7 % 2);
}

