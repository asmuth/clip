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
#include "domain.h"
#include <assert.h>
#include <iostream>
#include "utils/algo.h"

namespace plotfx {

static const double kDefaultLogBase = 10;

DomainConfig::DomainConfig() :
    kind(DomainKind::AUTO),
    min_auto_snap_zero(false),
    inverted(false),
    padding(0.1f) {}

void domain_fit_continuous(const Series& data_raw, DomainConfig* domain) {
  auto data = series_to_float(data_raw);

  for (const auto& d : data) {
    if (!domain->min_auto || *domain->min_auto > d) {
      domain->min_auto = std::optional<double>(d);
    }
    if (!domain->max_auto || *domain->max_auto < d) {
      domain->max_auto = std::optional<double>(d);
    }
  }
}

void domain_fit_categorical(const Series& data, DomainConfig* domain) {
  for (const auto& d : data) {
    if (domain->map.count(d) > 0) {
      continue;
    }

    domain->map.emplace(d, domain->categories.size());
    domain->categories.emplace_back(d);
  }
}

void domain_fit_kind(const Series& data, DomainConfig* domain) {
  if (series_is_numeric(data)) {
    domain->kind = DomainKind::LINEAR;
  } else {
    domain->kind = DomainKind::CATEGORICAL;
  }
}

void domain_fit(const Series& data, DomainConfig* domain) {
  if (domain->kind == DomainKind::AUTO) {
    domain_fit_kind(data, domain);
  }

  switch (domain->kind) {
    case DomainKind::LINEAR:
    case DomainKind::LOGARITHMIC:
      return domain_fit_continuous(data, domain);
    case DomainKind::CATEGORICAL:
      return domain_fit_categorical(data, domain);
  }
}

size_t domain_cardinality(const DomainConfig& domain) {
  switch (domain.kind) {
    case DomainKind::CATEGORICAL:
      return domain.categories.size();
    default:
      return 1;
  }
}

double domain_min(const DomainConfig& domain) {
  auto min = domain.min.value_or(domain.min_auto.value_or(0));
  auto max = domain.max.value_or(domain.max_auto.value_or(0));

  double min_auto = 0;
  if (!domain.min_auto_snap_zero || min < 0) {
    min_auto = min - (max - min) * domain.padding;
  }

  return domain.min.value_or(min_auto);
}

double domain_max(const DomainConfig& domain) {
  auto min = domain.min.value_or(domain.min_auto.value_or(0));
  auto max = domain.max.value_or(domain.max_auto.value_or(0));
  double max_auto = max + (max - min) * domain.padding;
  return domain.max.value_or(max_auto);
}

double domain_translate_linear(
    const DomainConfig& domain,
    const Value& v) {
  double min = domain_min(domain);
  double max = domain_max(domain);

  auto vf = value_to_float(v);
  auto vt = (vf - min) / (max - min);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return std::clamp(vt, 0.0, 1.0);
}

double domain_translate_log(
    const DomainConfig& domain,
    const Value& v) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  auto vf = value_to_float(v) - min;
  if (vf > 1.0) {
    vf = log(vf) / log(log_base);
  } else {
    vf = 0;
  }

  auto vt = vf / range_log;
  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return std::clamp(vt, 0.0, 1.0);
}

double domain_translate_categorical(
    const DomainConfig& domain,
    const Value& v) {
  double category_count = domain.categories.size();

  size_t vi = 0;
  if (auto vm = domain.map.find(v); vm != domain.map.end()) {
    vi = vm->second;
  }

  auto vt = (vi / category_count) + (0.5 / category_count);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return std::clamp(vt, 0.0, 1.0);
}

double domain_translate(
    const DomainConfig& domain,
    const Value& value) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_translate_linear(domain, value);
    case DomainKind::LOGARITHMIC:
      return domain_translate_log(domain, value);
    case DomainKind::CATEGORICAL:
      return domain_translate_categorical(domain, value);
    default:
      return std::numeric_limits<double>::quiet_NaN();
  }

  return 0.0f;
}

std::vector<double> domain_translate(
    const DomainConfig& domain,
    const Series& series) {
  std::vector<double> values;
  for (const auto& v : series) {
    values.push_back(domain_translate(domain, v));
  }

  return values;
}

Value domain_untranslate_linear(const DomainConfig& domain, double vt) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return value_from_float(min + (max - min) * vt);
}

Value domain_untranslate_log(const DomainConfig& domain, double vt) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return value_from_float(min + pow(log_base, vt * range_log));
}

Value domain_untranslate_categorical(
    const DomainConfig& domain,
    double vt) {
  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  std::string v;
  size_t vidx = vt * domain.categories.size();
  if (vidx >= 0 && vidx < domain.categories.size()) {
    v = domain.categories[vidx];
  }

  return v;
}

Value domain_untranslate(
    const DomainConfig& domain,
    double value) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_untranslate_linear(domain, value);
    case DomainKind::LOGARITHMIC:
      return domain_untranslate_log(domain, value);
    case DomainKind::CATEGORICAL:
      return domain_untranslate_categorical(domain, value);
  }

  return {};
}

Series domain_untranslate(
    const DomainConfig& domain,
    const std::vector<double>& values) {
  Series s;
  for (const auto& v : values) {
    s.emplace_back(domain_untranslate(domain, v));
  }

  return s;
}

ReturnCode domain_configure(
    const plist::Property& prop,
    DomainConfig* domain) {
  auto args = plist::flatten(prop);
  for (const auto& prop : args) {
    if (prop == "linear") {
      domain->kind = DomainKind::LINEAR;
      continue;
    }

    if (prop == "log" ||
        prop == "logarithmic") {
      domain->kind = DomainKind::LOGARITHMIC;
      continue;
    }

    if (prop == "categorical") {
      domain->kind = DomainKind::CATEGORICAL;
      continue;
    }

    if (prop == "invert" ||
        prop == "inverted") {
      domain->inverted = true;
      continue;
    }

    return ERROR_INVALID_ARGUMENT;
  }

  return OK;
}

} // namespace plotfx

