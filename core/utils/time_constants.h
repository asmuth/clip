/**
 * This file is part of the "clip" project
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
#ifndef _STX_TIME_CONSTANTS_H
#define _STX_TIME_CONSTANTS_H
#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>

constexpr const uint64_t kMicrosPerMilli = 1000;
constexpr const uint64_t kMicrosPerSecond = 1000000;
constexpr const uint64_t kMillisPerSecond = 1000;
constexpr const uint64_t kSecondsPerMinute = 60;
constexpr const uint64_t kMiillisPerMinute = kSecondsPerMinute * kMillisPerSecond;
constexpr const uint64_t kMicrosPerMinute = kSecondsPerMinute * kMicrosPerSecond;
constexpr const uint64_t kMinutesPerHour = 60;
constexpr const uint64_t kSecondsPerHour = kSecondsPerMinute * kMinutesPerHour;
constexpr const uint64_t kMillisPerHour = kSecondsPerHour * kMillisPerSecond;
constexpr const uint64_t kMicrosPerHour = kSecondsPerHour * kMicrosPerSecond;
constexpr const uint64_t kHoursPerDay = 24;
constexpr const uint64_t kSecondsPerDay = kSecondsPerHour * kHoursPerDay;
constexpr const uint64_t kMillisPerDay = kSecondsPerDay * kMillisPerSecond;
constexpr const uint64_t kMicrosPerDay = kSecondsPerDay * kMicrosPerSecond;
constexpr const uint64_t kDaysPerWeek = 7;
constexpr const uint64_t kSecondsPerWeek = kSecondsPerDay * kDaysPerWeek;
constexpr const uint64_t kMillisPerWeek = kSecondsPerWeek * kMicrosPerSecond;
constexpr const uint64_t kMicrosPerWeek = kSecondsPerWeek * kMicrosPerSecond;
constexpr const uint64_t kDaysPerYear = 365;
constexpr const uint64_t kSecondsPerYear = kDaysPerYear * kSecondsPerDay;
constexpr const uint64_t kMillisPerYear = kDaysPerYear * kMillisPerDay;
constexpr const uint64_t kMicrosPerYear = kDaysPerYear * kMicrosPerDay;

#endif
