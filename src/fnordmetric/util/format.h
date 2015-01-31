/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_FORMAT_H
#define _FNORDMETRIC_FORMAT_H
#include <string.h>
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace util {
namespace format {

std::string numberToHuman(double value);
std::string svalueToHuman(const query::SValue& value);
std::string formatTime(fnordmetric::TimeType time, const char* fmt = nullptr);
std::string formatTimeWithRange(fnordmetric::TimeType time, int range);

// FIXPAUL clean up...
template <typename T>
std::string toHuman(T value) {
  query::SValue sval(value);
  return sval.toString();
}

}
}
}
#endif
