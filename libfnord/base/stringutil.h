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
#include <vector>

namespace fnord {

class StringUtil {
public:

  /**
   * Stringify the provided value
   *
   * @param value any value
   * @return a string representation of the value
   */
  template <typename T>
  static std::string toString(T value);

  /**
   * Remove trailing slashes from the pointed to string
   *
   * @param str the string to remove trailing slashes from
   */
  static void stripTrailingSlashes(std::string* str);

  /**
   * Check if the provided string begins with the provided prefix
   *
   * @param str the string to check
   * @param prefix the prefix to check for
   * @return true if the string begins with the provided prefix, false otherwise
   */
  static bool beginsWith(
      const std::string& str,
      const std::string& prefix);

  /**
   * Check if the provided string ends with the provided suffix
   *
   * @param str the string to check
   * @param suffix the suffix to check for
   * @return true if the string ends with the provided suffix, false otherwise
   */
  static bool endsWith(
      const std::string& str,
      const std::string& suffix);

  /**
   * Replace all occurences of pattern with replacement in str
   *
   * @param str the str that should be modified
   * @param pattern the substring to replace
   * @param replacement the string with which to replace occurences of pattern
   */
  static void replaceAll(
      std::string* str,
      const std::string& pattern,
      const std::string& replacement = "");

  /**
   * Split the provided string on the provided pattern and return the parts
   */
  static std::vector<std::string> split(
      const std::string& str,
      const std::string& pattern);

  /**
   * Print the contents of the pointed to memory as a series of hexadecimal
   * bytes (hexdump):
   *
   * Example:
   *   StringUtil::hexPrint("\x17\x23\x42\x01", 4);
   *   // returns "17 23 42 01"
   *
   * @param data the data to print
   * @param size the size of the data in bytes
   * @param separate_bytes if true, insert a whitespace character between bytes
   * @param reverse_byte_order if true, print the data from last to first byte
   * @return the data formatted as a human readable hex string
   */
  static std::string hexPrint(
      const void* data,
      size_t size,
      bool separate_bytes = true,
      bool reverse_byte_order = false);

  /**
   * Format the provided string using StringUtil::format and print to stdou
   *
   * Example:
   *    StringUtil::puts("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // prints "The teapot is 23.5 pounds"
   *
   * @param fmt the format string
   * @param values... the values to insert into the format string
   */
  template <typename... T>
  static void puts(const char* fmt, T... values);

  /**
   * Insert values into a string with placeholders. This method will throw an
   * exception if an invalid placeholder is referenced
   *
   * Example:
   *    StringUtil::format("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // returns "The teapot is 23.5 pounds"
   *
   * @param fmt the format string
   * @param values... the values to insert into the format string
   * @return the format string with placeholders inserted
   */
  template <typename... T>
  static std::string format(const char* fmt, T... values);

  /**
   * Insert values into a string with placeholders. This method will throw an
   * exception if an invalid placeholder is referenced
   *
   * Example:
   *    StringUtil::format("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // returns "The teapot is 23.5 pounds"
   *
   * @param fmt the format string
   * @param values... the values to insert into the format string
   * @return the format string with placeholders inserted
   */
  template <typename... T>
  static std::string format(const std::string& fmt, T... values);

  /**
   * Insert values into a string with placeholders. This method will throw an
   * exception if an invalid placeholder is referenced
   *
   * Example:
   *    StringUtil::format("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // returns "The teapot is 23.5 pounds"
   *
   * @param fmt the format string
   * @param values the values to insert into the format string
   * @return the format string with placeholders inserted
   */
  static std::string formatv(const char* fmt, std::vector<std::string> values);

  /**
   * Insert values into a string with placeholders. This method will throw an
   * exception if an invalid placeholder is referenced
   *
   * Example:
   *    StringUtil::format("The $0 is $1 $2", "teapot", 23.5, "pounds");
   *    // returns "The teapot is 23.5 pounds"
   *
   * @param fmt the format string
   * @param values the values to insert into the format string
   * @return the format string with placeholders inserted
   */
  static std::string formatv(
      const std::string& fmt,
      std::vector<std::string> values);

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
   *
   * @param value the value to format
   * @return the number formatted as a string using the SI prefixes
   */
  template <typename T>
  static std::string formatNumberMetric(T value);

  /**
   * Format a number using the scientific notation
   *
   * @param value the value to format
   * @return the number formatted as a string using the scientific notation
   */
  template <typename T>
  static std::string formatNumberScientific(T value);

protected:

  template <typename ValueType, typename... T>
  static void formatImpl(
      std::string* scratch,
      int argn,
      ValueType value,
      T... values);

  template <typename ValueType>
  static void formatImpl(
      std::string* scratch,
      int argn,
      ValueType value);

};

} // namespace fnord

#include "stringutil_impl.h"
#endif
