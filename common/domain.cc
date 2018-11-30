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

namespace plotfx {

DomainConfig::DomainConfig() :
    kind(DomainKind::LINEAR),
    inverted(false),
    padding(0.0f) {}

void domain_fit(const std::vector<double>& data, DomainConfig* domain) {
  bool fit_min = !domain->min;
  bool fit_max = !domain->max;

  for (const auto& d : data) {
    if (fit_min && (!domain->min || *domain->min > d)) {
      domain->min = std::optional<double>(d);
    }
    if (fit_max && (!domain->max || *domain->max < d)) {
      domain->max = std::optional<double>(d);
    }
  }

  auto range = domain->max.value_or(0.0f) - domain->min.value_or(0.0f);
  if (fit_max) {
    domain->max = std::optional<double>(
        domain->max.value_or(0.0f) + range * domain->padding);
  }

  if (fit_min) {
    domain->min = std::optional<double>(
        domain->min.value_or(0.0f) - range * domain->padding);
  }
}

double domain_translate(const DomainConfig& domain, double v) {
  auto min = domain.min.value_or(0.0f);
  auto max = domain.max.value_or(0.0f);

  auto vt = 0.0;
  switch (domain.kind) {
    case DomainKind::LINEAR:
      vt = (v - min) / (max - min);
      break;
  }

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return vt;
}

double domain_untranslate(const DomainConfig& domain, double vt) {
  auto min = domain.min.value_or(0.0f);
  auto max = domain.max.value_or(0.0f);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  auto v = 0.0;
  switch (domain.kind) {
    case DomainKind::LINEAR:
      v = min + (max - min) * vt;
      break;
  }

  return v;
}

namespace chart {

/*
const char AnyDomain::kDimensionLetters[] = "xyz";
const int AnyDomain::kDefaultNumTicks = 8;
const double AnyDomain::kDefaultDomainPadding = 0.1;

template <> Domain<int64_t>*
    Domain<int64_t>::mkDomain() {
  return new ContinuousDomain<int64_t>();
}

template <> Domain<double>*
    Domain<double>::mkDomain() {
  return new ContinuousDomain<double>();
}

template <> Domain<UnixTime>*
    Domain<UnixTime>::mkDomain() {
  return new TimeDomain();
}

template <> Domain<std::string>* Domain<std::string>::mkDomain() {
  return new DiscreteDomain<std::string>();
}
*/

}
}

