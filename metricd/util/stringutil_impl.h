/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

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

