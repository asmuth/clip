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
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "return_code.h"
#include "sexpr.h"
#include "color_scheme.h"
#include "format.h"

namespace fviz {

enum class ScaleKind {
  LINEAR, LOGARITHMIC, CATEGORICAL
};

struct ScaleLimitHints {
  std::optional<double> min_value;
  std::optional<double> max_value;
};

struct ScaleConfig {
  ScaleConfig();
  ScaleKind kind;
  std::optional<double> min;
  std::optional<double> max;
  std::optional<double> log_base;
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

std::function<double (double)> scale_translate_fn(const ScaleConfig& domain);

double scale_untranslate(
    const ScaleConfig& domain,
    double data);

std::vector<double> scale_untranslate(
    const ScaleConfig& domain,
    const std::vector<double>& data);

ReturnCode scale_configure_kind(
    const Expr* expr,
    ScaleConfig* domain);

ReturnCode scale_layout_linear(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step,
    std::optional<double> begin,
    std::optional<double> end);

ReturnCode scale_layout_subdivide(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    uint32_t divisions);

ReturnCode scale_layout_discrete(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout);

ReturnCode scale_layout_categorical(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout);

ReturnCode scale_configure_layout(
    const Expr* expr,
    ScaleLayoutFn* layout);

} // namespace fviz

