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
