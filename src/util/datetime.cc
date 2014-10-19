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
#include <fnordmetric/util/datetime.h>
#include <fnordmetric/util/wallclock.h>

namespace fnord {
namespace util {

DateTime::DateTime(uint64_t utc_time) : utc_time_(utc_time) {}

DateTime::operator uint64_t() const {
  return utc_time_;
}

DateTime::operator double() const {
  return utc_time_;
}

DateTime DateTime::epoch() {
  return DateTime(0);
}

DateTime DateTime::now() {
  return DateTime(WallClock::unixMicros());
}

std::string DateTime::toString(const char* fmt) const {
  struct tm tm;
  time_t tt = utc_time_ / 1000000;
  localtime_r(&tt, &tm); // FIXPAUL

  char buf[256]; // FIXPAUL
  buf[0] = 0;
  strftime(buf, sizeof(buf), fmt, &tm);

  return std::string(buf);
}

}
}

fnord::util::DateTime
    std::numeric_limits<fnord::util::DateTime>::min() {
  return fnord::util::DateTime::epoch();
}

fnord::util::DateTime
    std::numeric_limits<fnord::util::DateTime>::max() {
  return fnord::util::DateTime(std::numeric_limits<uint64_t>::max());
}
