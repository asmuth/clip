/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/stringutil.h>

namespace fnord {

template <>
std::string StringUtil::toString(std::string value) {
  return value;
}

template <>
std::string StringUtil::toString(const char* value) {
  return value;
}

template <>
std::string StringUtil::toString(double value) {
  char buf[128]; // FIXPAUL
  *buf = 0;
  snprintf(buf, sizeof(buf), "%g", value);
  return buf;
}

void StringUtil::stripTrailingSlashes(std::string* str) {
  while (str->back() == '/') {
    str->pop_back();
  }
}

void StringUtil::replaceAll(
    std::string* str,
    const std::string& pattern,
    const std::string& replacement) {
  if (str->size() == 0) {
    return;
  }

  auto cur = 0;
  while((cur = str->find(pattern, cur)) != std::string::npos) {
    str->replace(cur, pattern.size(), replacement);
    cur += replacement.size();
  }
}

bool StringUtil::beginsWith(const std::string& str, const std::string& prefix) {
  if (str.length() < prefix.length()) {
    return false;
  }

  return str.compare(
      0,
      prefix.length(),
      prefix) == 0;
}


bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }

  return str.compare(
      str.length() - suffix.length(),
      suffix.length(),
      suffix) == 0;
}

std::string StringUtil::hexPrint(
    const void* data,
    size_t size,
    bool sep /* = true */,
    bool reverse /* = fase */) {
  static const char hexTable[] = "0123456789abcdef";

  std::string str;

  if (reverse) {
    for (int i = size - 1; i >= 0; --i) {
      if (sep && i < size - 1) { str += " "; }
      auto byte = ((const unsigned char*) data)[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  } else {
    for (int i = 0; i < size; ++i) {
      if (sep && i > 0) { str += " "; }
      auto byte = ((const unsigned char*) data)[i];
      str += hexTable[(byte & 0xf0) >> 4];
      str += hexTable[byte & 0x0f];
    }
  }

  return str;
}

std::string StringUtil::formatv(
    const char* fmt,
    std::vector<std::string> values) {
  std::string str = fmt;

  for (int i = 0; i < values.size(); ++i) {
    StringUtil::replaceAll(
        &str,
        "$" + std::to_string(i),
        StringUtil::toString(values[i]));
  }

  return str;
}

} // namespace fnord
