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
#include "CivilTime.h"
#include <string>
#include <ctime>

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
