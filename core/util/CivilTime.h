/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#pragma once
#include "stdtypes.h"
#include "option.h"

using namespace signaltk; // FIXME

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

#include "CivilTime_impl.h"
