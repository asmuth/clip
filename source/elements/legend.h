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
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include "graphics/layer.h"
#include "core/environment.h"
#include "core/layout.h"

namespace plotfx {

const std::string LEGEND_DEFAULT = "default";

enum class LegendPlacement {
  OFF,
  INSIDE,
  OUTSIDE
};

using LegendItemList = std::vector<std::string>;

struct LegendConfig {
  LegendConfig();
  std::string key;
  Color text_color;
  Color border_color;
  FontInfo font;
  Measure padding_horiz;
  Measure padding_vert;
  Measure padding_item_horiz;
  Measure padding_item_vert;
  Measure margins[4];
  Measure item_margins[4];
  LegendPlacement placement;
  HAlign position_horiz;
  VAlign position_vert;
  std::string title;
  std::vector<std::string> items;
  std::vector<Color> colors;
};

namespace legend {

ReturnCode draw(
    const LegendConfig& config,
    const LayoutInfo& clip,
    Layer* layer);

ReturnCode layout(
    const LegendConfig& config,
    const Layer& layer,
    LayoutInfo* layout);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    LegendConfig* config);


} // namespace legned
} // namespace plotfx

