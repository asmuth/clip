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

namespace plotfx {

static const double kDefaultLogBase = 10;

DomainConfig::DomainConfig() :
    kind(DomainKind::LINEAR),
    min_auto_snap_zero(false),
    inverted(false),
    padding(0.0f) {}

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
  std::set<std::string> cache;
  for (const auto& d : domain->categories) {
    cache.insert(d);
  }

  for (const auto& d : data) {
    if (cache.count(d) > 0) {
      continue;
    }

    domain->categories.emplace_back(d);
    cache.insert(d);
  }
}

void domain_fit(const Series& data, DomainConfig* domain) {
  switch (domain->kind) {
    case DomainKind::LINEAR:
    case DomainKind::LOGARITHMIC:
      return domain_fit_continuous(data, domain);
    case DomainKind::CATEGORICAL:
      return domain_fit_categorical(data, domain);
  }
}

double domain_min(const DomainConfig& domain) {
  auto min = domain.min.value_or(domain.min_auto.value_or(0));
  auto max = domain.max.value_or(domain.max_auto.value_or(0));

  auto min_auto = 0;
  if (!domain.min_auto_snap_zero || min < 0) {
    min_auto = min - (max - min) * domain.padding;
  }

  return domain.min.value_or(min_auto);
}

double domain_max(const DomainConfig& domain) {
  auto min = domain.min.value_or(domain.min_auto.value_or(0));
  auto max = domain.max.value_or(domain.max_auto.value_or(0));
  auto max_auto = max + (max - min) * domain.padding;
  return domain.max.value_or(max_auto);
}

std::vector<double> domain_translate_linear(
    const DomainConfig& domain,
    const Series& series) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);

  std::vector<double> mapped;
  for (const auto& v : series) {
    auto vf = value_to_float(v);
    auto vt = (vf - min) / (max - min);

    if (domain.inverted) {
      vt = 1.0 - vt;
    }

    mapped.push_back(vt);
  }

  return mapped;
}

std::vector<double> domain_translate_log(
    const DomainConfig& domain,
    const Series& series) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  std::vector<double> mapped;
  for (const auto& v : series) {
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

    mapped.push_back(vt);
  }

  return mapped;
}

std::vector<double> domain_translate_categorical(
    const DomainConfig& domain,
    const Series& series) {
  std::unordered_map<std::string, double> cache;
  for (size_t i = 0; i < domain.categories.size(); ++i) {
    cache.emplace(domain.categories[i], double(i));
  }

  double category_count = domain.categories.size();

  std::vector<double> mapped;
  for (const auto& v : series) {
    auto vt = (cache[v] / category_count) + (0.5 / category_count);

    if (domain.inverted) {
      vt = 1.0 - vt;
    }

    mapped.push_back(vt);
  }

  return mapped;
}

std::vector<double> domain_translate(
    const DomainConfig& domain,
    const Series& series) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_translate_linear(domain, series);
    case DomainKind::LOGARITHMIC:
      return domain_translate_log(domain, series);
    case DomainKind::CATEGORICAL:
      return domain_translate_categorical(domain, series);
  }

  return {};
}

Series domain_untranslate_linear(const DomainConfig& domain, std::vector<double> values) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);

  Series s;
  for (auto vt : values) {
    if (domain.inverted) {
      vt = 1.0 - vt;
    }

    s.emplace_back(value_from_float(min + (max - min) * vt));
  }

  return s;
}

Series domain_untranslate_log(const DomainConfig& domain, std::vector<double> values) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  Series s;
  for (auto vt : values) {
    if (domain.inverted) {
      vt = 1.0 - vt;
    }

    s.emplace_back(value_from_float(min + pow(log_base, vt * range_log)));
  }

  return s;
}

Series domain_untranslate_categorical(
    const DomainConfig& domain,
    std::vector<double> values) {
  Series s;
  for (auto vt : values) {
    if (domain.inverted) {
      vt = 1.0 - vt;
    }

    std::string v;
    size_t vidx = vt * domain.categories.size();
    if (vidx >= 0 && vidx < domain.categories.size()) {
      v = domain.categories[vidx];
    }

    s.emplace_back(std::move(v));
  }

  return s;
}

Series domain_untranslate(
    const DomainConfig& domain,
    const std::vector<double>& values) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_untranslate_linear(domain, values);
    case DomainKind::LOGARITHMIC:
      return domain_untranslate_log(domain, values);
    case DomainKind::CATEGORICAL:
      return domain_untranslate_categorical(domain, values);
  }

  return {};
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

