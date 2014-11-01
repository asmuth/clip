/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/stringutil.h>

namespace fnord {
namespace util {

void StringUtil::stripTrailingSlashes(std::string* str) {
  while (str->back() == '/') {
    str->pop_back();
  }
}

std::string StringUtil::hexPrint(
    void* data,
    size_t size,
    bool sep /* = true */,
    bool reverse /* = fase */) {
  static const char hexTable[] = "0123456789abcdef";

  std::string str;

  if (reverse) {
    for (int i = size - 1; i >= 0; --i) {
      if (sep && i > 0) { str += " "; }
      auto byte = ((unsigned char*) data)[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  } else {
    for (int i = 0; i < size; ++i) {
      if (sep && i > 0) { str += " "; }
      auto byte = ((unsigned char*) data)[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  }

  return str;
}

template <>
std::string StringUtil::debugPrint<bool>(const bool& value) {
  return value == true ? "true" : "false";
}

template <>
std::string StringUtil::debugPrint<int>(const int& value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::debugPrint<unsigned int>(const unsigned int& value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::debugPrint<unsigned long>(const unsigned long& value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::debugPrint<unsigned long long>(
    const unsigned long long& value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::debugPrint<double>(const double& value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::debugPrint<std::string>(const std::string& value) {
  return value;
}

template <>
std::string StringUtil::debugPrint<char const*>(char const* const& value) {
  return std::string(value);
}

template <>
std::string StringUtil::debugPrint<void*>(void* const& value) {
  return "<ptr>";
}

template <>
std::string StringUtil::debugPrint<const void*>(void const* const& value) {
  return "<ptr>";
}

}
}
