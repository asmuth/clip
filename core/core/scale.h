/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "utils/return_code.h"
#include "sexpr.h"
#include "source/color_scheme.h"
#include "source/data_model.h"

namespace plotfx {

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

using ScaleLayoutFn = std::function<void (
    const DomainConfig& domain,
    ScaleLayout*)>;

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

ReturnCode domain_configure(
    const plist::Property& prop,
    DomainConfig* domain);

ReturnCode scale_layout_linear(
    const DomainConfig& domain,
    ScaleLayout* layout,
    double step,
    std::optional<double> align);

ReturnCode scale_layout_subdivide(
    const DomainConfig& domain,
    ScaleLayout* layout,
    uint32_t divisions);

ReturnCode scale_layout_discrete(
    const DomainConfig& domain,
    ScaleLayout* layout);

ReturnCode configure_scale_layout(
    const plist::Property& prop,
    ScaleLayoutFn* layout);

} // namespace plotfx

