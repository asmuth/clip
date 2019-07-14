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
#include "environment.h"
#include "core/format.h"
#include "graphics/font_lookup.h"

using namespace std::placeholders;

namespace fviz {

Environment::Environment() :
    screen_width(Unit::UNIT, 1200),
    screen_height(Unit::UNIT, 480),
    dpi(96),
    background_color(Color::fromRGB(1,1,1)),
    text_color(Color::fromRGB(.2,.2,.2)),
    border_color(Color::fromRGB(.2,.2,.2)),
    font_size(from_pt(11, dpi)) {}

ReturnCode environment_setup_defaults(Environment* env) {
  if (!font_load(DefaultFont::HELVETICA_REGULAR, &env->font)) {
    return ReturnCode::error(
        "EARG",
        "unable to find default sans-sans font (Helvetica/Arial)");
  }

  return OK;
}

} // namespace fviz

