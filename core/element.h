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
#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include "graphics/layer.h"
#include "sexpr.h"
#include "return_code.h"

namespace fviz {
struct Environment;
struct LayoutInfo;

struct Element;
using ElementRef = std::shared_ptr<Element>;

using ElementConfigureFn = std::function<
    ReturnCode (
        const Environment& env,
        const Expr* expr,
        ElementRef* elem)>;

using ElementDrawFn = std::function<
    ReturnCode (
        const LayoutInfo& layout,
        Layer* layer)>;

using ElementSizeHintFn = std::function<
    ReturnCode (
        const Layer& layer,
        const std::optional<double> max_width,
        const std::optional<double> max_height,
        double* min_width,
        double* min_height)>;

struct Element {
  ElementDrawFn draw;
  ElementSizeHintFn size_hint;
};


} // namespace fviz

