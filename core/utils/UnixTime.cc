/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string>
#include <ctime>
#include "UnixTime.h"
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
