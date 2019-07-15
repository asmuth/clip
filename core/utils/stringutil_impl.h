/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fviz {

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


} // namespace fviz
