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
#include <stdlib.h>
#include <plist/plist.h>
#include <graphics/layer.h>
#include <core/scale.h>
#include <element.h>
#include <config_helpers.h>

namespace plotfx {
namespace gridlines {

using GridlineLayout = std::function<ReturnCode (
    const DomainConfig& domain,
    std::vector<double>*)>;

struct GridlineDefinition {
  DomainConfig scale_x;
  DomainConfig scale_y;
  ScaleLayoutFn layout_x;
  ScaleLayoutFn layout_y;
  Measure line_width;
  Color line_color;
  LayoutSettings layout;
};

ReturnCode draw(
    const GridlineDefinition& config,
    const LayoutState& clip,
    Layer* layer);

ReturnCode layout(
    const GridlineDefinition& config,
    const Layer& layer,
    LayoutState* layout);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    GridlineDefinition* config);

} // namespace gridlines
} // namespace plotfx

