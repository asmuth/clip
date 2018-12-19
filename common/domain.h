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
#include "plist/plist.h"
#include "common/color_scheme.h"
#include "common/data_frame.h"

namespace plotfx {

enum class DomainKind {
  AUTO, LINEAR, LOGARITHMIC, CATEGORICAL
};

struct DomainConfig {
  DomainConfig();
  DomainKind kind;
  bool inverted;
  std::optional<double> min;
  std::optional<double> min_auto;
  bool min_auto_snap_zero;
  std::optional<double> max;
  std::optional<double> max_auto;
  std::optional<double> log_base;
  std::vector<std::string> categories;
  std::unordered_map<std::string, double> map;
  double padding;
};

using DomainMap = std::unordered_map<std::string, DomainConfig>;

static const std::string SCALE_DEFAULT_X = "x";
static const std::string SCALE_DEFAULT_Y = "y";

const DomainConfig* domain_find(
    const DomainMap& map,
    const std::string& key);

DomainConfig* domain_find(
    DomainMap* map,
    const std::string& key);

void domain_fit(const Series& data, DomainConfig* domain);

void domain_fit(
    const DataFrame& data,
    const std::string& data_key,
    const std::string& key,
    DomainMap* domains);

size_t domain_cardinality(const DomainConfig& domain);

double domain_translate(
    const DomainConfig& domain,
    const Value& value);

std::vector<double> domain_translate(
    const DomainConfig& domain,
    const Series& series);

Value domain_untranslate(
    const DomainConfig& domain,
    double data);

Series domain_untranslate(
    const DomainConfig& domain,
    const std::vector<double>& data);

ReturnCode domain_configure(
    const plist::Property& prop,
    DomainConfig* domain);

} // namespace plotfx

