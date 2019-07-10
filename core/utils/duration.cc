/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *   - Christian Parpart <trapni@dawanda.com>
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
