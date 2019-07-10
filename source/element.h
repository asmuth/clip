/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include "sexpr.h"
#include "utils/return_code.h"
#include "core/format.h"
#include "graphics/geometry.h"

namespace plotfx {
struct Layer;
struct LayoutInfo;
struct LayoutSettings;
struct Document;
struct DataContext;
struct Environment;

using plist::PropertyList;

template <typename T>
using ElementConfigureAsFn = std::function<ReturnCode (
    const plist::PropertyList&,
    const Environment&,
    T*)>;

using ElementDrawFn = std::function<ReturnCode (const LayoutInfo&, Layer*)>;

template <typename T>
using ElementDrawAsFn = std::function<ReturnCode (
    const T&,
    const LayoutInfo&,
    Layer*)>;

using ElementReflowFn = std::function<ReturnCode (
    const Layer&,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height)>;

template <typename T>
using ElementReflowAsFn = std::function<ReturnCode (
    const T&,
    const Layer&,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height)>;

struct Element {
  virtual ~Element() = default;
  virtual const LayoutSettings& layout_settings() const = 0;
  int32_t z_index() const;
  ElementReflowFn reflow;
  ElementDrawFn draw;
};

template <typename T>
struct ElementInstance : public Element {
  T config;

  const LayoutSettings& layout_settings() const override {
    return config.layout;
  }

};

using ElementRef = std::shared_ptr<Element>;

} // namespace plotfx

