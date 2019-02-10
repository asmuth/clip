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
#include "plist/plist.h"
#include "utils/return_code.h"
#include "core/format.h"
#include "core/layout.h"

namespace plotfx {
struct Layer;
struct LayoutState;
struct Document;
struct DataContext;
struct Environment;

using plist::PropertyList;

using ElementDrawFn = std::function<ReturnCode (const LayoutState&, Layer*)>;

template <typename T>
using ElementDrawAsFn = std::function<ReturnCode (const T&, const LayoutState&, Layer*)>;

template <typename T>
using ElementConfigureAsFn = std::function<ReturnCode (
    const plist::PropertyList&,
    const Environment&,
    T*)>;

template <typename T>
using ElementDrawAsFn = std::function<ReturnCode (const T&, const LayoutState&, Layer*)>;

using ElementLayoutFn = std::function<ReturnCode (const Layer&, LayoutState*)>;

template <typename T>
using ElementLayoutAsFn = std::function<ReturnCode (const T&, const Layer&, LayoutState*)>;

struct Element {
  virtual ~Element() = default;
  virtual const LayoutSettings& layout_settings() const = 0;
  ElementLayoutFn layout;
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

