/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "libpaint/util/format.h"
#include "libpaint/util/time.h"

namespace fnordmetric {

ReturnCode parsePointInTime(const std::string& str, uint64_t* timestamp) {
  /* "now" */
  if (str == "now") {
    *timestamp = WallClock::unixMicros();
    return ReturnCode::success();
  }

  /* -<duration> */
  if (StringUtil::beginsWith(str, "-") && str.size() > 1 && isdigit(str[1])) {
    uint64_t offset = 0;
    auto rc = parseDuration(str.substr(1), &offset);
    if (rc.isSuccess()) {
      *timestamp = WallClock::unixMicros() - offset;
      return ReturnCode::success();
    }
  }

  /* raw timestamps */
  if (StringUtil::isDigitString(str)) {
    static const uint64_t bound_milli = 30000000000;
    static const uint64_t bound_micro = 30000000000000;
    try {
      uint64_t ts = std::stoull(str);

      if (ts < bound_milli) {
        *timestamp = ts * kMicrosPerSecond;
        return ReturnCode::success();
      }

      if (ts > bound_milli && ts < bound_micro) {
        *timestamp = ts * 1000;
        return ReturnCode::success();
      }

      if (ts > bound_micro) {
        *timestamp = ts;
        return ReturnCode::success();
      }
    } catch (...) {
      /* fallthrough */
    }
  }

  auto iso_time = ISO8601::parse(str);
  if (!iso_time.isEmpty()) {
    *timestamp = UnixTime(iso_time.get()).unixMicros();
    return ReturnCode::success();
  }

  auto civil_time = CivilTime::parseString(str);
  if (!civil_time.isEmpty()) {
    *timestamp = UnixTime(civil_time.get()).unixMicros();
    return ReturnCode::success();
  }

  if (StringUtil::beginsWith(str, "0000-00-00")) {
    *timestamp = 0;
    return ReturnCode::success();
  }

  return ReturnCode::errorf("EARG", "invalid time: '$0'", str);
}

ReturnCode parseDuration(const String& str, uint64_t* duration) {
  size_t sz;
  auto value = std::stoull(str, &sz);
  const auto& time_suffix = str.substr(sz);

  if (time_suffix == "s" ||
      time_suffix == "sec" ||
      time_suffix == "secs" ||
      time_suffix == "second" ||
      time_suffix == "seconds") {
    *duration = value * kMicrosPerSecond;
    return ReturnCode::success();
  }

  if (time_suffix == "m" ||
      time_suffix == "min" ||
      time_suffix == "mins" ||
      time_suffix == "minute" ||
      time_suffix == "minutes") {
    *duration = value * kMicrosPerMinute;
    return ReturnCode::success();
  }

  if (time_suffix == "h" ||
      time_suffix == "hour" ||
      time_suffix == "hours") {
    *duration = value * kMicrosPerHour;
    return ReturnCode::success();
  }

  if (time_suffix == "d" ||
      time_suffix == "day" ||
      time_suffix == "days") {
    *duration = value * kMicrosPerDay;
    return ReturnCode::success();
  }

  if (time_suffix == "w" ||
      time_suffix == "week" ||
      time_suffix == "weeks") {
    *duration = value * kMicrosPerWeek;
    return ReturnCode::success();
  }

  if (time_suffix == "y" ||
      time_suffix == "year" ||
      time_suffix == "years") {
    *duration = value * kMicrosPerYear;
    return ReturnCode::success();
  }

  return ReturnCode::errorf("EARG", "invalid duration: $0", str);
}

} // namespace fnordmetric

