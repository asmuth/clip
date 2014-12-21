/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <ctime>
#include "fnord/base/datetime.h"
#include "fnord/base/inspect.h"
#include "fnord/base/wallclock.h"
#include "fnord/base/stringutil.h"

namespace fnord {

DateTime::DateTime() : utc_micros_(WallClock::unixMicros()) {}

DateTime::DateTime(uint64_t utc_time) : utc_micros_(utc_time) {}

DateTime& DateTime::operator=(const DateTime& other) {
  utc_micros_ = other.utc_micros_;
  tz_offset_ = other.tz_offset_;
  return *this;
}

bool DateTime::operator==(const DateTime& other) const {
  return utc_micros_ < other.utc_micros_;
}

bool DateTime::operator<(const DateTime& other) const {
  return utc_micros_ < other.utc_micros_;
}

bool DateTime::operator>(const DateTime& other) const {
  return utc_micros_ > other.utc_micros_;
}

bool DateTime::operator<=(const DateTime& other) const {
  return utc_micros_ <= other.utc_micros_;
}

bool DateTime::operator>=(const DateTime& other) const {
  return utc_micros_ >= other.utc_micros_;
}

DateTime::operator uint64_t() const {
  return utc_micros_;
}

DateTime::operator double() const {
  return utc_micros_;
}

uint64_t DateTime::unixMicros() const {
  return utc_micros_;
}

DateTime DateTime::epoch() {
  return DateTime(0);
}

DateTime DateTime::now() {
  return DateTime(WallClock::unixMicros());
}

DateTime DateTime::daysFromNow(double days) {
  return DateTime(WallClock::unixMicros() + (days * kMicrosPerDay));
}

std::string DateTime::toString(const char* fmt) const {
  struct tm tm;
  time_t tt = utc_micros_ / 1000000;
  localtime_r(&tt, &tm); // FIXPAUL

  char buf[256]; // FIXPAUL
  buf[0] = 0;
  strftime(buf, sizeof(buf), fmt, &tm);

  return std::string(buf);
}

template <>
std::string StringUtil::toString(DateTime value) {
  return value.toString();
}

template <>
std::string inspect(const DateTime& value) {
  return value.toString();
}

}

fnord::DateTime
    std::numeric_limits<fnord::DateTime>::min() {
  return fnord::DateTime::epoch();
}

fnord::DateTime
    std::numeric_limits<fnord::DateTime>::max() {
  return fnord::DateTime(std::numeric_limits<uint64_t>::max());
}
