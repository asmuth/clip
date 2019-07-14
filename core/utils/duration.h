/**
 * This file is part of the "fviz" project
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

#ifndef _STX_DURATION_H
#define _STX_DURATION_H

#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>
#include "time_constants.h"

class Duration {
private:
  enum class ZeroType { Zero };

public:
  constexpr static ZeroType Zero = ZeroType::Zero;

  /**
   * Creates a new Duration of zero microseconds.
   */
  constexpr Duration(ZeroType);

  /**
   * Create a new Duration
   *
   * @param microseconds the duration in microseconds
   */
  constexpr Duration(uint64_t microseconds);

  constexpr bool operator==(const Duration& other) const;
  constexpr bool operator!=(const Duration& other) const;
  constexpr bool operator<(const Duration& other) const;
  constexpr bool operator>(const Duration& other) const;
  constexpr bool operator<=(const Duration& other) const;
  constexpr bool operator>=(const Duration& other) const;
  constexpr bool operator!() const;

  constexpr Duration operator+(const Duration& other) const;

  /**
   * Return the represented duration in microseconds
   */
  constexpr explicit operator uint64_t() const;

  /**
   * Return the represented duration in microseconds
   */
  constexpr explicit operator double() const;

  /**
   * Return the represented duration in microseconds
   */
  constexpr uint64_t microseconds() const noexcept;

  /**
   * Return the represented duration in milliseconds
   */
  constexpr uint64_t milliseconds() const noexcept;

  /**
   * Return the represented duration in seconds
   */
  constexpr uint64_t seconds() const noexcept;

  constexpr uint64_t minutes() const noexcept;
  constexpr uint64_t hours() const noexcept;
  constexpr uint64_t days() const noexcept;

  static inline Duration fromDays(uint64_t v);
  static inline Duration fromHours(uint64_t v);
  static inline Duration fromMinutes(uint64_t v);
  static inline Duration fromSeconds(uint64_t v);
  static inline Duration fromMilliseconds(uint64_t v);
  static inline Duration fromMicroseconds(uint64_t v);
  static inline Duration fromNanoseconds(uint64_t v);

protected:
  uint64_t micros_;
};

std::string inspect(const Duration& value);

#include "duration_impl.h"
#endif
