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
#include "graphics/color.h"
#include "return_code.h"

#include <vector>

namespace fviz {

using ColorPalette = std::vector<Color>;

ColorPalette color_palette_default();

ReturnCode color_palette_default(const std::string& name, ColorPalette* colors);

} // namespace fviz
