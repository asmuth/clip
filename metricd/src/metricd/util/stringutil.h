/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <set>
#include <vector>
#include <locale>

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

  template <typename... T>
  static std::vector<std::string> toStringV(T... values);

  template <typename H, typename... T>
  static void toStringVImpl(
      std::vector<std::string>* target,
      H value,
      T... values);

  static void toStringVImpl(std::vector<std::string>* target);

  /**
   * Remove trailing slashes from the pointed to string
   *
   * @param str the string to remove trailing slashes from
   */
  static void stripTrailingSlashes(std::string* str);

  /**
   * Remove all characters except [A-Za-z0-9_-.] from the string
   *
   * @param str the string to remove chars from
   * @return the stripped string
   */
  static std::string stripShell(const std::string& str);

  /**
   * JSON Escape
   *
   * @param str the string to escape
   * @return the escaped string
   */
  static std::string jsonEscape(const std::string& str);

  /**
   * JSON Unescape
   *
   * @param str the string to unescape
   * @return the unescaped string
   */
  static std::string jsonUnescape(const std::string& str);

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
   * Lexicographically compare the strings s1 and s2. Return an integer greater
   * than, equal to, or less than 0, according as the string s1 is greater than,
   * equal to, or less than the string s2.
   *
   * The comparison is done using unsigned characters, so that `\200' is greater
   * than `\0'.
   */
  static int compare(
      const char* s1,
      size_t s1_len,
      const char* s2,
      size_t s2_len);

  /**
   * Check if the provided string contains only 0-9a-fA-f
   *
   * @param str the string to check
   */
  static bool isHexString(const std::string& str);

  /**
   * Check if the provided string consists only of 0-9a-zA-Z
   *
   * @param chr the char to check
   */
  static bool isAlphanumeric(const std::string& string);

  /**
   * Check if the provided char is one of 0-9a-zA-Z
   *
   * @param chr the char to check
   */
  static bool isAlphanumeric(char chr);

  /**
   * Check if the provided string consists only of 0-9a-zA-Z-_.
   *
   * @param chr the char to check
   */
  static bool isShellSafe(const std::string& string);

  /**
   * Check if the provided char is one of 0-9a-zA-Z-_.
   *
   * @param chr the char to check
   */
  static bool isShellSafe(char chr);

  /**
   * Check if the provided string consists only of digits
   *
   * @param str the string to check
   * @return true if the string consists only of digits, false otherwise
   */
  static bool isDigitString(const std::string& str);
  static bool isDigitString(const char* begin, const char* end);

  /**
   * Check if the provided string matches the pattern ^-?[0-9]+([,\.][0-9])$
   *
   * @param str the string to check
   * @return true if the string consists only of digits, false otherwise
   */
  static bool isNumber(const std::string& str);
  static bool isNumber(const char* begin, const char* end);

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
   * Join the provided string array with the provided join string
   */
  static std::string join(const std::vector<std::string>& list, const std::string& join);
  static std::string join(const std::set<std::string>& list, const std::string& join);

  /**
   * Converts the provided string to all lowercase
   *
   * @param str the str that should be modified
   */
  static void toLower(std::string* str);

  /**
   * Converts the provided string to all uppercase
   *
   * @param str the str that should be modified
   */
  static void toUpper(std::string* str);

  /**
   * Finds the first occurence of the provided character in the string. Returns
   * std::string::npos if the character does not occur in the string.
   *
   * @param str the str that should be searched
   * @param chr the char to search for
   * @return the position of the first occurence of chr or std::string::npos
   */
  static size_t find(const std::string& str, char chr);

  /**
   * Finds the last occurence of the provided character in the string. Returns
   * std::string::npos if the character does not occur in the string.
   *
   * @param str the str that should be searched
   * @param chr the char to search for
   * @return the position of the last occurence of chr or std::string::npos
   */
  static size_t findLast(const std::string& str, char chr);

  /**
   * Returns true iff str contains the subject or is equal ot the subject
   *
   * @param str the str to be searched
   * @param subject the substring to search for
   */
  static bool includes(const std::string& str, const std::string& subject);

  /**
   * Returns true iff str contains the subject or is equal ot the subject,
   * matches in a case-insensitive fashion
   *
   * @param str the str to be searched
   * @param subject the substring to search for
   */
  static bool includesi(const std::string& str, const std::string& subject);

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

#include "stringutil_impl.h"
