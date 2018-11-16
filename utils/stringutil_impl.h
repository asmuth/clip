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
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace signaltk {

template <typename H, typename... T>
void StringUtil::toStringVImpl(
    std::vector<std::string>* target,
    H value,
    T... values) {
  target->emplace_back(toString(value));
  toStringVImpl(target, values...);
}

template <typename... T>
std::vector<std::string> StringUtil::toStringV(T... values) {
  std::vector<std::string> target;
  toStringVImpl(&target, values...);
  return target;
}

template <typename ValueType, typename... T>
void StringUtil::formatImpl(
    std::string* scratch,
    int argn,
    ValueType value,
    T... values) {
  StringUtil::replaceAll(
      scratch,
      "$" + std::to_string(argn),
      StringUtil::toString(value));

  formatImpl(scratch, argn + 1, values...);
}

template <typename ValueType>
void StringUtil::formatImpl(
    std::string* scratch,
    int argn,
    ValueType value) {
  StringUtil::replaceAll(
      scratch,
      "$" + std::to_string(argn),
      StringUtil::toString(value));
}

template <typename... T>
std::string StringUtil::format(const char* fmt, T... values) {
  std::string str = fmt;
  StringUtil::formatImpl(&str, 0, values...);
  return str;
}

template <typename... T>
std::string StringUtil::format(const std::string& fmt, T... values) {
  std::string str = fmt;
  StringUtil::formatImpl(&str, 0, values...);
  return str;
}

template <typename T>
std::string StringUtil::formatNumberMetric(T orig_value) {
  char buf[128];
  size_t len = 0;
  double value = orig_value; // FIXPAUL
  auto abs_value = fabs(value);

  if (abs_value == 0){
    len = snprintf(buf, sizeof(buf), "0");
  }

  else if (abs_value < 0.000000001){
    len = snprintf(buf, sizeof(buf), "%.1fp", value * 1000000000000);
  }

  else if (abs_value < 0.000001){
    len = snprintf(buf, sizeof(buf), "%.1fn", value * 1000000000);
  }

  else if (abs_value < 0.001){
    len = snprintf(buf, sizeof(buf), "%.1fμ", value * 1000000);
  }

  else if (abs_value < 0.1){
    len = snprintf(buf, sizeof(buf), "%.1fm", value * 1000);
  }

  else if (abs_value < 10){
    len = snprintf(buf, sizeof(buf), "%.2f", value);
  }

  else if (abs_value < 1000) {
    len = snprintf(buf, sizeof(buf), "%.1f", value);
  }

  else if (abs_value < 1000000) {
    len = snprintf(buf, sizeof(buf), "%.1fK", value / 1000);
  }

  else if (abs_value < 1000000000) {
    len = snprintf(buf, sizeof(buf), "%.1fM", value / 1000000);
  }

  else if (abs_value < 1000000000000) {
    len = snprintf(buf, sizeof(buf), "%.1fG", value / 1000000000);
  }

  else {
    len = snprintf(buf, sizeof(buf), "%.1fT", value / 1000000000000);
  }

  return std::string(buf, len);
}


} // namespace signaltk
