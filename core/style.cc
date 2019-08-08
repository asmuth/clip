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
#include "style.h"
#include "graphics/brush.h"

namespace fviz {

FillStyle fill_style_solid(const Color& c) {
  return [c] (const auto& p, auto l) -> ReturnCode {
    fillPath(l, p, c);
    return OK;
  };
}

FillStyle fill_style_none() {
  return [] (const auto& p, auto l) -> ReturnCode {
    return OK;
  };
}

} // namespace fviz

