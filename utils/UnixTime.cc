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
#include <string>
#include <ctime>
#include "UnixTime.h"
#include "inspect.h"
#include "wallclock.h"
#include "stringutil.h"
#include "ISO8601.h"

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

Option<UnixTime> UnixTime::parseString(
    const String& str,
    const char* fmt /* = "%Y-%m-%d %H:%M:%S" */) {
  return UnixTime::parseString(str.data(), str.size(), fmt);
}

Option<UnixTime> UnixTime::parseString(
    const char* str,
    size_t strlen,
    const char* fmt /* = "%Y-%m-%d %H:%M:%S" */) {
  auto ct = CivilTime::parseString(str, strlen, fmt);
  if (ct.isEmpty()) {
    return None<UnixTime>();
  } else {
    return Some(UnixTime(ct.get()));
  }
}

template <>
std::string StringUtil::toString(UnixTime value) {
  return value.toString();
}

//template <>
//std::string inspect(const UnixTime& value) {
//  return value.toString();
//}

UnixTime std::numeric_limits<UnixTime>::min() {
  return UnixTime::epoch();
}

UnixTime std::numeric_limits<UnixTime>::max() {
  return UnixTime(std::numeric_limits<uint64_t>::max());
}
