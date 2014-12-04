/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_STRINGUTIL_H_
#define _FNORD_BASE_STRINGUTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {

class StringUtil {
public:

  /**
   * Stringify the provided value
   */
  template <typename T>
  static std::string toString(T value);

  static void stripTrailingSlashes(std::string* str);

  static bool beginsWith(
      const std::string& str,
      const std::string& prefix);

  static bool endsWith(
      const std::string& str,
      const std::string& suffix);

  static void replaceAll(
      std::string* str,
      const std::string& pattern,
      const std::string& replacement);

  static std::string hexPrint(
      const void* data,
      size_t size,
      bool separate_bytes = true,
      bool reverse_byte_order = false);

  /**
   * Insert values into a string with placeholders.
   *
   * Example:
   *    StringUtil::format("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // returns "The teapot is 23.5 pounds"
   *
   */
  template <typename... T>
  static std::string format(const char* fmt, T... values);

  /**
   * Format a number using these SI prefixes:
   *
   *   tera  T  1000000000000
   *   giga  G  1000000000
   *   mega  M  1000000
   *   kilo  k  1000
   *   milli m  0.001
   *   micro μ  0.000001
   *   nano  n  0.000000001
   *   pico  p  0.000000000001
   */
  template <typename T>
  static std::string formatNumberMetric(T value);

  /**
   * Format a number using the scientific notation
   */
  template <typename T>
  static std::string formatNumberScientific(T value);

};

} // namespace fnord

#endif
