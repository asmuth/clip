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
#include "layer.h"

namespace fviz {
class Rasterizer;

ReturnCode layer_bind_img(
    double width,
    double height,
    double dpi,
    Measure font_size,
    const Color& background_color,
    std::function<Status (const unsigned char* data, size_t len)> submit,
    LayerRef* layer);

ReturnCode layer_bind_png(
    double width,
    double height,
    double dpi,
    Measure font_size,
    const Color& background_color,
    std::function<Status (const std::string&)> submit,
    LayerRef* layer);

} // namespace fviz

