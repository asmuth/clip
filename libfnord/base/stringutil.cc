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

std::string StringUtil::hexPrint(
    const void* data,
    size_t size,
    bool sep /* = true */,
    bool reverse /* = fase */) {
  static const char hexTable[] = "0123456789abcdef";

  std::string str;

  if (reverse) {
    for (int i = size - 1; i >= 0; --i) {
      if (sep && i > 0) { str += " "; }
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

} // namespace fnord
