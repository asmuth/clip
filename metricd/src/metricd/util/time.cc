/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Christian Parpart
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <string>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#include "time.h"
#include "stringutil.h"

UnixTime WallClock::now() {
  return UnixTime(WallClock::getUnixMicros());
}

uint64_t WallClock::unixSeconds() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}

uint64_t WallClock::getUnixMillis() {
  return unixMillis();
}

uint64_t WallClock::unixMillis() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000llu + tv.tv_usec / 1000llu;
}

uint64_t WallClock::getUnixMicros() {
  return unixMicros();
}

uint64_t WallClock::unixMicros() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000llu + tv.tv_usec;
}

uint64_t MonotonicClock::now() {
#ifdef __MACH__
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  return std::uint64_t(mts.tv_sec) * 1000000 + mts.tv_nsec / 1000;
#else
  timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    std::cerr << "clock_gettime(CLOCK_MONOTONIC) failed\n";
    abort();
  } else {
    return std::uint64_t(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
  }
#endif
}

UnixTime::UnixTime() :
    utc_micros_(WallClock::unixMicros()) {}

UnixTime::UnixTime(const CivilTime& civil) {
  uint64_t days = civil.day() - 1;

  for (auto i = 1970; i < civil.year(); ++i) {
    days += 365 + ISO8601::isLeapYear(i);
  }

  for (auto i = 1; i < civil.month(); ++i) {
    days += ISO8601::daysInMonth(civil.year(), i);
  }

  utc_micros_ =
      days * kMicrosPerDay +
      civil.hour() * kMicrosPerHour +
      civil.minute() * kMicrosPerMinute +
      civil.second() * kMicrosPerSecond +
      civil.millisecond() * 1000 +
      civil.offset() * kMicrosPerSecond * -1;
}

UnixTime& UnixTime::operator=(const UnixTime& other) {
  utc_micros_ = other.utc_micros_;
  return *this;
}

UnixTime UnixTime::now() {
  return UnixTime(WallClock::unixMicros());
}

UnixTime UnixTime::daysFromNow(double days) {
  return UnixTime(WallClock::unixMicros() + (days * kMicrosPerDay));
}

std::string UnixTime::toString(const char* fmt) const {
  struct tm tm;
  time_t tt = utc_micros_ / 1000000;
  gmtime_r(&tt, &tm); // FIXPAUL

  char buf[256]; // FIXPAUL
  buf[0] = 0;
  strftime(buf, sizeof(buf), fmt, &tm);

  return std::string(buf);
}

template <>
std::string StringUtil::toString(UnixTime value) {
  return value.toString();
}

UnixTime std::numeric_limits<UnixTime>::min() {
  return UnixTime::epoch();
}

UnixTime std::numeric_limits<UnixTime>::max() {
  return UnixTime(std::numeric_limits<uint64_t>::max());
}

Option<CivilTime> CivilTime::parseString(
    const String& str,
    const char* fmt /* = "%Y-%m-%d %H:%M:%S" */) {
  return CivilTime::parseString(str.data(), str.size(), fmt);
}

Option<CivilTime> CivilTime::parseString(
    const char* str,
    size_t strlen,
    const char* fmt /* = "%Y-%m-%d %H:%M:%S" */) {
  struct tm t;

  // FIXME strptime doesn't handle time zone offsets
  if (strptime(str, fmt, &t) == NULL) {
    return None<CivilTime>();
  } else {
    CivilTime ct;
    ct.setSecond(t.tm_sec);
    ct.setMinute(t.tm_min);
    ct.setHour(t.tm_hour);
    ct.setDay(t.tm_mday);
    ct.setMonth(t.tm_mon + 1);
    ct.setYear(t.tm_year + 1900);
    return Some(ct);
  }
}

void CivilTime::setYear(uint16_t value) {
  year_ = value;
}

void CivilTime::setMonth(uint8_t value) {
  month_ = value;
}

void CivilTime::setDay(uint8_t value) {
  day_ = value;
}

void CivilTime::setHour(uint8_t value) {
  hour_ = value;
}

void CivilTime::setMinute(uint8_t value) {
  minute_ = value;
}

void CivilTime::setSecond(uint8_t value) {
  second_ = value;
}

void CivilTime::setMillisecond(uint16_t value) {
  millisecond_ = value;
}

void CivilTime::setOffset(int32_t value) {
  offset_ = value;
}

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

