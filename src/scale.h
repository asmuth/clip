/**
 * This file is part of the "clip" project
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
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <unordered_map>
#include "return_code.h"
#include "sexpr.h"
#include "format.h"
#include "utils/option.h"

namespace clip {

enum class ScaleKind {
  LINEAR, LOGARITHMIC, CATEGORICAL
};

struct ScaleLimitHints {
  Option<double> min_value;
  Option<double> max_value;
};

struct ScaleConfig {
  ScaleConfig();
  ScaleKind kind;
  Option<double> min;
  Option<double> max;
  double log_base;
  bool inverted;
  double padding;
  std::shared_ptr<ScaleLimitHints> limit_hints;
  std::vector<std::string> categories;
  std::unordered_map<std::string, size_t> categories_map;
};

struct ScaleLayout {
  std::vector<double> positions;
  std::vector<std::string> labels;
};

using ScaleLayoutFn = std::function<
    void (
        const ScaleConfig&,
        const Formatter&,
        ScaleLayout*)>;

void scale_fit(double value, ScaleConfig* domain);

double scale_min(const ScaleConfig& domain);
double scale_max(const ScaleConfig& domain);

double scale_translate(
    const ScaleConfig& domain,
    double value);

double scale_translate_magnitude(
    const ScaleConfig& domain,
    double value);

double scale_translate_categorical(
    const ScaleConfig& domain,
    size_t index);

std::function<double (double)> scale_translate_fn(const ScaleConfig& domain);

std::function<double (double)> scale_translate_magnitude_fn(const ScaleConfig& domain);

double scale_untranslate(
    const ScaleConfig& domain,
    double data);

std::vector<double> scale_untranslate(
    const ScaleConfig& domain,
    const std::vector<double>& data);

ReturnCode scale_configure_kind(
    const Expr* expr,
    ScaleConfig* domain);

ReturnCode scale_layout_linear_interval(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step,
    double begin,
    double end);

ReturnCode scale_layout_linear_alignat(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step,
    double align);

ReturnCode scale_layout_linear_align(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step);

ReturnCode scale_layout_linear(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step);

ReturnCode scale_layout_exponential_steps(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double base,
    size_t steps);

ReturnCode scale_layout_exponential(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double base);

ReturnCode scale_layout_subdivide(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    uint32_t divisions);

ReturnCode scale_layout_categorical(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout);

ReturnCode scale_layout_categorical_bounds(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout);

ReturnCode scale_layout_none(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout);

ReturnCode scale_configure_layout(
    const Expr* expr,
    ScaleLayoutFn* layout);

void scale_configure_layout_defaults(
    const ScaleConfig& config,
    ScaleLayoutFn* label_placement,
    ScaleLayoutFn* tick_placement);

} // namespace clip

