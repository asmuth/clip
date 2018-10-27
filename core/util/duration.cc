/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *   - Christian Parpart <trapni@dawanda.com>
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
#include "duration.h"
#include "stringutil.h"
#include <sstream>

std::string inspect(const Duration& value) {
  unsigned years = value.days() / kDaysPerYear;
  unsigned days = value.days() % kDaysPerYear;;
  unsigned hours = value.hours() % kHoursPerDay;
  unsigned minutes = value.minutes() % kMinutesPerHour;
  unsigned seconds = value.seconds() % kSecondsPerMinute;
  unsigned msecs = value.milliseconds () % kMillisPerSecond;

  std::stringstream sstr;
  int i = 0;

  if (years)
    sstr << years << " years";

  if (days) {
    if (i++) sstr << ' ';
    sstr << days << " days";
  }

  if (hours) {
    if (i++) sstr << ' ';
    sstr << hours << " hours";
  }

  if (minutes) {
    if (i++) sstr << ' ';
    sstr << minutes << " minutes";
  }

  if (seconds) {
    if (i++) sstr << ' ';
    sstr << seconds << " seconds";
  }

  if (msecs) {
    if (i) sstr << ' ';
    sstr << msecs << "ms";
  }

  return sstr.str();
}

//template<>
//std::string StringUtil::toString<Duration>(Duration duration) {
//  return inspect(duration);
//}
//
//template<>
//std::string StringUtil::toString<const Duration&>(const Duration& duration) {
//  return inspect(duration);
//}
