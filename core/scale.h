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
#include "utils/return_code.h"
#include "sexpr.h"
#include "color_scheme.h"
#include "data_model.h"

namespace fviz {

enum class DomainKind {
  LINEAR, LOGARITHMIC
};

struct DomainLimitHints {
  std::optional<double> min_value;
  std::optional<double> max_value;
};

struct DomainConfig {
  DomainConfig();
  DomainKind kind;
  std::optional<double> min;
  std::optional<double> max;
  std::optional<double> log_base;
  bool inverted;
  double padding;
  std::shared_ptr<DomainLimitHints> limit_hints;
};

struct ScaleLayout {
  std::vector<double> ticks;
  std::vector<double> labels;
};

using ScaleLayoutFn = std::function<void (ScaleLayout*)>;

void domain_fit(double value, DomainConfig* domain);

double domain_min(const DomainConfig& domain);
double domain_max(const DomainConfig& domain);

double domain_translate(
    const DomainConfig& domain,
    double value);

std::function<double (double)> domain_translate_fn(const DomainConfig& domain);

double domain_untranslate(
    const DomainConfig& domain,
    double data);

std::vector<double> domain_untranslate(
    const DomainConfig& domain,
    const std::vector<double>& data);

//ReturnCode domain_configure(
//    const plist::Property& prop,
//    DomainConfig* domain);

ReturnCode scale_layout_linear(
    const DomainConfig& domain,
    ScaleLayout* layout,
    double step,
    std::optional<double> align);

ReturnCode scale_layout_subdivide(
    ScaleLayout* layout,
    uint32_t divisions);

ReturnCode scale_layout_discrete(
    const DomainConfig& domain,
    ScaleLayout* layout);

//ReturnCode configure_scale_layout(
//    const plist::Property& prop,
//    ScaleLayoutFn* layout);

} // namespace fviz

