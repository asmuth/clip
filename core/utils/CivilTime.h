/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
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
#pragma once
#include "stdtypes.h"
#include "option.h"

using namespace fviz; // FIXME

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
