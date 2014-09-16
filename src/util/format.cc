/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <ctime>
#include "math.h"
#include "format.h"

namespace fnordmetric {
namespace util {
namespace format {

std::string numberToHuman(double value) {
  char buf[256];
  size_t len = 0;

  auto abs_value = fabs(value);

  if (abs_value == 0){
    len = snprintf(buf, sizeof(buf), "0");
  }

  else if (abs_value < 0.000000001){
    len = snprintf(buf, sizeof(buf), "%.1fp", value * 1000000000000);
  }

  else if (abs_value < 0.000001){
    len = snprintf(buf, sizeof(buf), "%.1fn", value * 1000000000);
  }

  else if (abs_value < 0.001){
    len = snprintf(buf, sizeof(buf), "%.1fμ", value * 1000000);
  }

  else if (abs_value < 0.1){
    len = snprintf(buf, sizeof(buf), "%.1fm", value * 1000);
  }

  else if (abs_value < 10){
    len = snprintf(buf, sizeof(buf), "%.2f", value);
  }

  else if (abs_value < 1000) {
    len = snprintf(buf, sizeof(buf), "%.1f", value);
  }

  else if (abs_value < 1000000) {
    len = snprintf(buf, sizeof(buf), "%.1fK", value / 1000);
  }

  else if (abs_value < 1000000000) {
    len = snprintf(buf, sizeof(buf), "%.1fM", value / 1000000);
  }

  else if (abs_value < 1000000000000) {
    len = snprintf(buf, sizeof(buf), "%.1fG", value / 1000000000);
  }

  else {
    len = snprintf(buf, sizeof(buf), "%.1fT", value / 1000000000000);
  }

  return std::string(buf, len);
}

std::string formatTime(fnordmetric::TimeType time, char* fmt /* = nullptr */) {
  static char default_fmt[] = "%Y-%m-%d %H:%M:%S";

  struct tm tm;
  localtime_r(&time, &tm);

  char buf[256];
  buf[0] = 0;
  strftime(buf, sizeof(buf), fmt == nullptr ? default_fmt : fmt, &tm);

  return std::string(buf);
}

std::string formatTimeWithRange(fnordmetric::TimeType time, int range) {
  if (range < 60 * 60) {
    return formatTime(time, "%H:%M:%S");
  } else if (range < 60 * 60 * 24) {
    return formatTime(time, "%H:%M");
  } else {
    return formatTime(time, "%Y-%m-%d %H:%M");
  }
}

}
}
}
