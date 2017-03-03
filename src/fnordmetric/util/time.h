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
#pragma once
#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>
#include "fnordmetric/util/option.h"

constexpr const uint64_t kMicrosPerMilli = 1000;
constexpr const uint64_t kMicrosPerSecond = 1000000;
constexpr const uint64_t kMillisPerSecond = 1000;
constexpr const uint64_t kSecondsPerMinute = 60;
constexpr const uint64_t kMiillisPerMinute = kSecondsPerMinute * kMillisPerSecond;
constexpr const uint64_t kMicrosPerMinute = kSecondsPerMinute * kMicrosPerSecond;
constexpr const uint64_t kMinutesPerHour = 60;
constexpr const uint64_t kSecondsPerHour = kSecondsPerMinute * kMinutesPerHour;
constexpr const uint64_t kMillisPerHour = kSecondsPerHour * kMillisPerSecond;
constexpr const uint64_t kMicrosPerHour = kSecondsPerHour * kMicrosPerSecond;
constexpr const uint64_t kHoursPerDay = 24;
constexpr const uint64_t kSecondsPerDay = kSecondsPerHour * kHoursPerDay;
constexpr const uint64_t kMillisPerDay = kSecondsPerDay * kMillisPerSecond;
constexpr const uint64_t kMicrosPerDay = kSecondsPerDay * kMicrosPerSecond;
constexpr const uint64_t kDaysPerWeek = 7;
constexpr const uint64_t kSecondsPerWeek = kSecondsPerDay * kDaysPerWeek;
constexpr const uint64_t kMillisPerWeek = kSecondsPerWeek * kMicrosPerSecond;
constexpr const uint64_t kMicrosPerWeek = kSecondsPerWeek * kMicrosPerSecond;
constexpr const uint64_t kDaysPerYear = 365;
constexpr const uint64_t kSecondsPerYear = kDaysPerYear * kSecondsPerDay;
constexpr const uint64_t kMillisPerYear = kDaysPerYear * kMillisPerDay;
constexpr const uint64_t kMicrosPerYear = kDaysPerYear * kMicrosPerDay;

class UnixTime;
class CivilTime;

class WallClock {
public:
  static UnixTime now();
  static uint64_t unixSeconds();
  static uint64_t getUnixMillis();
  static uint64_t unixMillis();
  static uint64_t getUnixMicros();
  static uint64_t unixMicros();
};

class MonotonicClock {
public:
  static uint64_t now();
};

class UnixTime {
public:

  /**
   * Create a new UTC UnixTime instance with time = now
   */
  UnixTime();

  /**
   * Create a new UTC UnixTime instance
   *
   * @param timestamp the UTC microsecond timestamp
   */
  constexpr UnixTime(uint64_t utc_time);

  /**
   * Create a new UTC UnixTime instance from a civil time reference
   *
   * @param civil the civil time
   */
  UnixTime(const CivilTime& civil);

  /**
   * Parse time from the provided string
   *
   * @param str the string to parse
   * @param fmt the strftime format string (optional)
   */
  static Option<UnixTime> parseString(
      const String& str,
      const char* fmt = "%Y-%m-%d %H:%M:%S");

  /**
   * Parse time from the provided string
   *
   * @param str the string to parse
   * @param strlen the size of the string to parse
   * @param fmt the strftime format string (optional)
   */
  static Option<UnixTime> parseString(
      const char* str,
      size_t strlen,
      const char* fmt = "%Y-%m-%d %H:%M:%S");

  /**
   * Return a representation of the date as a string (strftime)
   *
   * @param fmt the strftime format string (optional)
   */
  std::string toString(const char* fmt = "%Y-%m-%d %H:%M:%S") const;

  UnixTime& operator=(const UnixTime& other);

  constexpr bool operator==(const UnixTime& other) const;
  constexpr bool operator!=(const UnixTime& other) const;
  constexpr bool operator<(const UnixTime& other) const;
  constexpr bool operator>(const UnixTime& other) const;
  constexpr bool operator<=(const UnixTime& other) const;
  constexpr bool operator>=(const UnixTime& other) const;

  /**
   * Cast the UnixTime object to a UTC unix microsecond timestamp represented as
   * an uint64_t
   */
  constexpr explicit operator uint64_t() const;

  /**
   * Cast the UnixTime object to a UTC unix microsecond timestamp represented as
   * a double
   */
  constexpr explicit operator double() const;

  /**
   * Return the represented date/time as a UTC unix microsecond timestamp
   */
  constexpr uint64_t unixMicros() const;

  /**
   * Return a new UnixTime instance with time 00:00:00 UTC, 1 Jan. 1970
   */
  static inline UnixTime epoch();

  /**
   * Return a new UnixTime instance with time = now
   */
  static inline UnixTime now();

  /**
   * Return a new UnixTime instance with time = now + days
   */
  static inline UnixTime daysFromNow(double days);

protected:

  /**
   * The utc microsecond timestamp of the represented moment in time
   */
  uint64_t utc_micros_;
};

/**
 * Class representing an instance of time in the gregorian calendar
 */
class CivilTime {
public:

  /**
   * Create a new CivilTime instance with all fields set to zero
   */
  constexpr CivilTime();

  /**
   * Create a new CivilTime instance with all fields set to zero
   */
  constexpr CivilTime(std::nullptr_t);

  /**
   * Parse time from the provided string
   *
   * @param str the string to parse
   * @param fmt the strftime format string (optional)
   */
  static Option<CivilTime> parseString(
      const String& str,
      const char* fmt = "%Y-%m-%d %H:%M:%S");

  /**
   * Parse time from the provided string
   *
   * @param str the string to parse
   * @param strlen the size of the string to parse
   * @param fmt the strftime format string (optional)
   */
  static Option<CivilTime> parseString(
      const char* str,
      size_t strlen,
      const char* fmt = "%Y-%m-%d %H:%M:%S");

  /**
   * Year including century / A.D. (eg. 1999)
   */
  constexpr uint16_t year() const;

  /**
   * Month [1-12]
   */
  constexpr uint8_t month() const;

  /**
   * Day of the month [1-31]
   */
  constexpr uint8_t day() const;

  /**
   * Hour [0-23]
   */
  constexpr uint8_t hour() const;

  /**
   * Hour [0-59]
   */
  constexpr uint8_t minute() const;

  /**
   * Second [0-60]
   */
  constexpr uint8_t second() const;

  /**
   * Millisecond [0-999]
   */
  constexpr uint16_t millisecond() const;

  /**
   * Timezone offset to UTC in seconds
   */
  constexpr int32_t offset() const;

  void setYear(uint16_t value);
  void setMonth(uint8_t value);
  void setDay(uint8_t value);
  void setHour(uint8_t value);
  void setMinute(uint8_t value);
  void setSecond(uint8_t value);
  void setMillisecond(uint16_t value);
  void setOffset(int32_t value);

protected:
  uint16_t year_;
  uint8_t month_;
  uint8_t day_;
  uint8_t hour_;
  uint8_t minute_;
  uint8_t second_;
  uint16_t millisecond_;
  int32_t offset_;
};

class ISO8601 {
public:

  static Option<CivilTime> parse(const String& str);

  static bool isLeapYear(uint16_t year);

  static uint8_t daysInMonth(uint16_t year, uint8_t month);

};

namespace std {
template <> class numeric_limits<UnixTime> {
public:
  static UnixTime max();
  static UnixTime min();
};
}

#include "time_impl.h"
