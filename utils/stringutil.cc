/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include <string>
#include <assert.h>
#include "bufferutil.h"
#include "stringutil.h"
#include "UTF8.h"

namespace signaltk {

void StringUtil::toStringVImpl(std::vector<std::string>* target) {}

template <>
std::string StringUtil::toString(std::string value) {
  return value;
}

template <>
std::string StringUtil::toString(const char* value) {
  return value;
}

template <>
std::string StringUtil::toString(char* value) {
  return value;
}

template <>
std::string StringUtil::toString(int value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(unsigned value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(unsigned short value) {
  return std::to_string(value);
}


template <>
std::string StringUtil::toString(long value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(unsigned long value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(long long value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(unsigned long long value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(unsigned char value) {
  return std::to_string(value);
}

template <>
std::string StringUtil::toString(void* value) {
  return "<ptr>";
}

template <>
std::string StringUtil::toString(const void* value) {
  return "<ptr>";
}

template <>
std::string StringUtil::toString(double value) {
  char buf[128]; // FIXPAUL
  *buf = 0;

  auto len = snprintf(buf, sizeof(buf), "%f", value);
  if (len < 0) {
    return std::string{};
  }

  while (len > 2 && buf[len - 1] == '0' && buf[len - 2] != '.') {
    buf[len--] = 0;
  }

  return String(buf, len);
}

template <>
std::string StringUtil::toString(bool value) {
  return value ? "true" : "false";
}

void StringUtil::ltrim(std::string* str) {
  while (str->front() == ' ') {
    str->erase(str->begin());
  }
}

void StringUtil::rtrim(std::string* str) {
  while (str->back() == ' ') {
    str->pop_back();
  }
}

void StringUtil::stripTrailingSlashes(std::string* str) {
  while (str->back() == '/') {
    str->pop_back();
  }
}

void StringUtil::chomp(std::string* str) {
  while (str->back() == '\n' || str->back() == '\r') {
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

  size_t cur = 0;
  while ((cur = str->find(pattern, cur)) != std::string::npos) {
    str->replace(cur, pattern.size(), replacement);
    cur += replacement.size();
  }
}

std::vector<std::string> StringUtil::split(
      const std::string& str,
      const std::string& pattern) {
  assert(!pattern.empty());
  std::vector<std::string> parts;

  size_t begin = 0;
  for (;;) {
    auto end = str.find(pattern, begin);

    if (end == std::string::npos) {
      parts.emplace_back(str.substr(begin, end));
      break;
    } else {
      parts.emplace_back(str.substr(begin, end - begin));
      begin = end + pattern.size();
    }
  }

  return parts;
}

String StringUtil::join(const Vector<String>& list, const String& join) {
  String out;

  for (size_t i = 0; i < list.size(); ++i) {
    if (i > 0) {
      out += join;
    }

    out += list[i];
  }

  return out;
}

String StringUtil::join(const Set<String>& list, const String& join) {
  String out;

  size_t i = 0;
  for (const auto& item : list) {
    if (++i > 1) {
      out += join;
    }

    out += item;
  }

  return out;
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

int StringUtil::compare(
    const char* s1,
    size_t s1_len,
    const char* s2,
    size_t s2_len) {
  for (; s1_len > 0 && s2_len > 0; s1++, s2++, --s1_len, --s2_len) {
    if (*s1 != *s2) {
      return (*(uint8_t *) s1 < *(uint8_t *) s2) ? -1 : 1;
    }
  }

  if (s1_len > 0) {
    return 1;
  }

  if (s2_len > 0) {
    return -1;
  }

  return 0;
}


bool StringUtil::isHexString(const std::string& str) {
  for (const auto& c : str) {
    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')) {
      continue;
    }

    return false;
  }

  return true;
}

bool StringUtil::isAlphanumeric(const std::string& str) {
  for (const auto& c : str) {
    if (!isAlphanumeric(c)) {
      return false;
    }
  }

  return true;
}

bool StringUtil::isAlphanumeric(char chr) {
  bool is_alphanum =
      (chr >= '0' && chr <= '9') ||
      (chr >= 'a' && chr <= 'z') ||
      (chr >= 'A' && chr <= 'Z');

  return is_alphanum;
}

bool StringUtil::isShellSafe(const std::string& str) {
  for (const auto& c : str) {
    if (!isShellSafe(c)) {
      return false;
    }
  }

  return true;
}

bool StringUtil::isShellSafe(char chr) {
  bool is_safe =
      (chr >= '0' && chr <= '9') ||
      (chr >= 'a' && chr <= 'z') ||
      (chr >= 'A' && chr <= 'Z') ||
      (chr == '_') ||
      (chr == '-') ||
      (chr == '.');

  return is_safe;
}

bool StringUtil::isDigitString(const std::string& str) {
  return isDigitString(str.data(), str.data() + str.size());
}

bool StringUtil::isDigitString(const char* begin, const char* end) {
  for (auto cur = begin; cur < end; ++cur) {
    if (!isdigit(*cur)) {
      return false;
    }
  }

  return true;
}

bool StringUtil::isNumber(const std::string& str) {
  return isNumber(str.data(), str.data() + str.size());
}

bool StringUtil::isNumber(const char* begin, const char* end) {
  auto cur = begin;

  if (cur < end && *cur == '-') {
    ++cur;
  }

  for (; cur < end; ++cur) {
    if (!isdigit(*cur)) {
      return false;
    }
  }

  if (cur < end && (*cur == '.' || *cur == ',')) {
    ++cur;
  }

  for (; cur < end; ++cur) {
    if (!isdigit(*cur)) {
      return false;
    }
  }

  return true;
}

void StringUtil::toLower(std::string* str) {
  auto& str_ref = *str;

  for (size_t i = 0; i < str_ref.length(); ++i) {
    str_ref[i] = std::tolower(str_ref[i]);
  }
}

void StringUtil::toUpper(std::string* str) {
  auto& str_ref = *str;

  for (size_t i = 0; i < str_ref.length(); ++i) {
    str_ref[i] = std::toupper(str_ref[i]);
  }
}

size_t StringUtil::find(const std::string& str, char chr) {
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == chr) {
      return i;
    }
  }

  return -1;
}

size_t StringUtil::findLast(const std::string& str, char chr) {
  for (int i = str.length() - 1; i >= 0; --i) {
    if (str[i] == chr) {
      return i;
    }
  }

  return -1;
}

bool StringUtil::includes(const std::string& str, const std::string& subject) {
  return str.find(subject) != std::string::npos;
}

bool StringUtil::includesi(const std::string& str, const std::string& subject) {
  auto str_i = str;
  StringUtil::toLower(&str_i);
  auto subject_i = subject;
  StringUtil::toLower(&subject_i);
  return includes(str_i, subject_i);
}

std::string StringUtil::hexPrint(
    const void* data,
    size_t size,
    bool sep /* = true */,
    bool reverse /* = fase */) {
  Buffer buf(data, size);
  return BufferUtil::hexPrint(&buf, sep, reverse);
}

std::string StringUtil::formatv(
    const char* fmt,
    std::vector<std::string> values) {
  std::string str = fmt;

  for (size_t i = 0; i < values.size(); ++i) {
    StringUtil::replaceAll(
        &str,
        "$" + std::to_string(i),
        StringUtil::toString(values[i]));
  }

  return str;
}

std::basic_string<char32_t> StringUtil::convertUTF8To32(
    const std::basic_string<char>& str) {
  std::basic_string<char32_t> out;

  const char* cur = str.data();
  const char* end = cur + str.length();
  char32_t chr;
  while ((chr = UTF8::nextCodepoint(&cur, end)) > 0) {
    out += chr;
  }

  return out;
}


std::basic_string<char16_t> StringUtil::convertUTF8To16(
    const std::basic_string<char>& str) {
  std::basic_string<char16_t> out;

  const char* cur = str.data();
  const char* end = cur + str.length();
  char16_t chr;
  while ((chr = UTF8::nextCodepoint(&cur, end)) > 0) {
    out += chr;
  }

  return out;
}

std::basic_string<char> StringUtil::convertUTF32To8(
    const std::basic_string<char32_t>& str) {
  String out;

  for (const auto& c : str) {
    UTF8::encodeCodepoint(c, &out);
  }

  return out;
}

std::basic_string<char> StringUtil::convertUTF16To8(
    const std::basic_string<char16_t>& str) {
  String out;

  for (const auto& c : str) {
    UTF8::encodeCodepoint(c, &out);
  }

  return out;
}

size_t StringUtil::countUTF8CodePoints(const std::string& str) {
  size_t count = 0;
  const char* cur = str.data();
  const char* end = cur + str.length();
  while (UTF8::nextCodepoint(&cur, end) != 0) {
    ++count;
  }

  return count;
}


String StringUtil::stripShell(const std::string& str) {
  String out;

  for (const auto& c : str) {
    if (isAlphanumeric(c) || c == '_' || c == '-' || c == '.') {
      out += c;
    }
  }

  return out;
}

} // namespace signaltk
