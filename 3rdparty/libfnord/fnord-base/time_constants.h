/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_TIME_CONSTANTS_H
#define _FNORD_TIME_CONSTANTS_H
#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>

namespace fnord {

constexpr const uint64_t kMicrosPerSecond = 1000000;
constexpr const uint64_t kMillisPerSecond = 1000000;
constexpr const uint64_t kSecondsPerMinute = 60;
constexpr const uint64_t kMinutesPerHour = 60;
constexpr const uint64_t kSecondsPerHour = kSecondsPerMinute * kMinutesPerHour;
constexpr const uint64_t kMillisPerHour = kSecondsPerHour * kMillisPerSecond;
constexpr const uint64_t kMicrosPerHour = kSecondsPerHour * kMicrosPerSecond;
constexpr const uint64_t kHoursPerDay = 24;
constexpr const uint64_t kSecondsPerDay = kSecondsPerHour * kHoursPerDay;
constexpr const uint64_t kMillisPerDay = kSecondsPerDay * kMillisPerSecond;
constexpr const uint64_t kMicrosPerDay = kSecondsPerDay * kMicrosPerSecond;

} // namespace fnord
#endif
