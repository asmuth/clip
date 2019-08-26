/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth, Christian Parpart
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
inline constexpr Duration::Duration(ZeroType)
    : micros_(0) {}

inline constexpr Duration::Duration(uint64_t microseconds)
    : micros_(microseconds) {}

constexpr bool Duration::operator==(const Duration& other) const {
  return micros_ == other.micros_;
}

constexpr bool Duration::operator!=(const Duration& other) const {
  return micros_ != other.micros_;
}

constexpr bool Duration::operator<(const Duration& other) const {
  return micros_ < other.micros_;
}

constexpr bool Duration::operator>(const Duration& other) const {
  return micros_ > other.micros_;
}

constexpr bool Duration::operator<=(const Duration& other) const {
  return micros_ <= other.micros_;
}

constexpr bool Duration::operator>=(const Duration& other) const {
  return micros_ >= other.micros_;
}

constexpr bool Duration::operator!() const {
  return micros_ == 0;
}

inline constexpr uint64_t Duration::microseconds() const noexcept {
  return micros_;
}

inline constexpr uint64_t Duration::milliseconds() const noexcept {
  return micros_ / kMillisPerSecond;
}

inline constexpr uint64_t Duration::seconds() const noexcept {
  return micros_ / kMicrosPerSecond;
}

inline constexpr Duration Duration::operator+(const Duration& other) const {
  return Duration(micros_ + other.micros_);
}

inline constexpr uint64_t Duration::minutes() const noexcept {
  return seconds() / kSecondsPerMinute;
}

inline constexpr uint64_t Duration::hours() const noexcept {
  return minutes() / kMinutesPerHour;
}

inline constexpr uint64_t Duration::days() const noexcept {
  return hours() / kHoursPerDay;
}

Duration Duration::fromDays(uint64_t v) {
  return Duration(v * kMicrosPerSecond * kSecondsPerDay);
}

Duration Duration::fromHours(uint64_t v) {
  return Duration(v * kMicrosPerSecond * kSecondsPerHour);
}

Duration Duration::fromMinutes(uint64_t v) {
  return Duration(v * kMicrosPerSecond * kSecondsPerMinute);
}

Duration Duration::fromSeconds(uint64_t v) {
  return Duration(v * kMicrosPerSecond);
}

Duration Duration::fromMilliseconds(uint64_t v) {
  return Duration(v * 1000);
}

Duration Duration::fromMicroseconds(uint64_t v) {
  return Duration(v);
}

Duration Duration::fromNanoseconds(uint64_t v) {
  return Duration(v / 1000);
}
