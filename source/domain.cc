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
    kind(DomainKind::LINEAR),
    inverted(false),
    padding(0),
    limit_hints(std::make_shared<DomainLimitHints>()) {}

void domain_fit(double value, DomainConfig* domain) {
  if (!domain->limit_hints->min_value || *domain->limit_hints->min_value > value) {
    domain->limit_hints->min_value = std::optional<double>(value);
  }
  if (!domain->limit_hints->max_value || *domain->limit_hints->max_value < value) {
    domain->limit_hints->max_value = std::optional<double>(value);
  }
}

double domain_min(const DomainConfig& domain) {
  auto min_auto = 1.0f;
  if (domain.limit_hints->min_value) {
    min_auto = *domain.limit_hints->min_value - domain.padding;
  }

  return domain.min.value_or(min_auto);
}

double domain_max(const DomainConfig& domain) {
  auto max_auto = 1.0f;
  if (domain.limit_hints->max_value) {
    max_auto = *domain.limit_hints->max_value + domain.padding;
  }

  return domain.max.value_or(max_auto);
}

double domain_translate_linear(
    const DomainConfig& domain,
    double v) {
  double min = domain_min(domain);
  double max = domain_max(domain);

  auto vt = (v - min) / (max - min);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return vt;
}

double domain_translate_log(
    const DomainConfig& domain,
    double v) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  auto vf = v - min;
  if (vf > 1.0) {
    vf = log(vf) / log(log_base);
  } else {
    vf = 0;
  }

  auto vt = vf / range_log;
  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return vt;
}

double domain_translate(
    const DomainConfig& domain,
    double value) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_translate_linear(domain, value);
    case DomainKind::LOGARITHMIC:
      return domain_translate_log(domain, value);
    default:
      return std::numeric_limits<double>::quiet_NaN();
  }

  return 0.0f;
}

std::function<double (double)> domain_translate_fn(const DomainConfig& domain) {
  return bind(&domain_translate, domain, std::placeholders::_1);
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

Value domain_untranslate(
    const DomainConfig& domain,
    double value) {
  switch (domain.kind) {
    case DomainKind::LINEAR:
      return domain_untranslate_linear(domain, value);
    case DomainKind::LOGARITHMIC:
      return domain_untranslate_log(domain, value);
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

    if (prop == "invert" ||
        prop == "inverted") {
      domain->inverted = true;
      continue;
    }

    return err_invalid_value(prop, {
      "linear",
      "log",
      "logarithmic",
      "inverted",
      "inverted"
    });
  }

  return OK;
}

} // namespace plotfx

