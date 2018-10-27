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
inline constexpr Duration::Duration(ZeroType)
    : micros_(0) {}

inline constexpr Duration::Duration(uint64_t microseconds)
    : micros_(microseconds) {}

inline Duration::Duration(const struct ::timeval& value)
    : micros_(value.tv_sec + value.tv_usec * kMicrosPerSecond) {}

inline Duration::Duration(const struct ::timespec& value)
    : micros_(value.tv_sec + value.tv_nsec * kMicrosPerSecond / 1000) {}

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

inline constexpr Duration::operator struct timeval() const {
#if defined(STX_OS_DARWIN)
  // OS/X plays in it's own universe. ;(
  return { static_cast<time_t>(micros_ / kMicrosPerSecond),
           static_cast<__darwin_suseconds_t>(micros_ % kMicrosPerSecond) };
#else
  return { static_cast<time_t>(micros_ / kMicrosPerSecond),
           static_cast<long>(micros_ % kMicrosPerSecond) };
#endif
}

inline constexpr Duration::operator struct timespec() const {
#if defined(STX_OS_DARWIN)
  // OS/X plays in it's own universe. ;(
  return { static_cast<time_t>(micros_ / kMicrosPerSecond),
           (static_cast<__darwin_suseconds_t>(micros_ % kMicrosPerSecond) * 1000) };
#else
  return { static_cast<time_t>(micros_ / kMicrosPerSecond),
           (static_cast<long>(micros_ % kMicrosPerSecond) * 1000) };
#endif
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
