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
#pragma once
#include <stdlib.h>
#include "graphics/color.h"

namespace fviz {

class ColorScheme {
public:

  ColorScheme(
      const std::vector<std::string>& colors = std::vector<std::string>{
          "#4572a7",
          "#aa4643",
          "#89a54e",
          "#80699b",
          "#3d96ae",
          "#db843d",
          }) :
          colors_(colors) {}

  Color get(size_t seq) const {
    Color c;
    c.parse(colors_[seq % colors_.size()]);
    return c;
  }

protected:
  std::vector<std::string> colors_;
};

} // namespace fviz

