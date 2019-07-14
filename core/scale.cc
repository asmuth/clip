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
#include "core/scale.h"
#include "core/sexpr_conv.h"
#include "core/sexpr_util.h"

#include <assert.h>
#include <iostream>
#include <functional>

using namespace std::placeholders;
namespace fviz {

static const double kDefaultLogBase = 10;
static const size_t kMaxTicks = 8192;

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
  auto min_auto = 0.0f;
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

double  domain_untranslate_linear(const DomainConfig& domain, double vt) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return min + (max - min) * vt;
}

double domain_untranslate_log(const DomainConfig& domain, double vt) {
  auto min = domain_min(domain);
  auto max = domain_max(domain);
  auto log_base = domain.log_base.value_or(kDefaultLogBase);
  double range = max - min;
  double range_log = log(range) / log(log_base);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return min + pow(log_base, vt * range_log);
}

double domain_untranslate(
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

std::vector<double> domain_untranslate(
    const DomainConfig& domain,
    const std::vector<double>& values) {
  std::vector<double> s;
  for (const auto& v : values) {
    s.emplace_back(domain_untranslate(domain, v));
  }

  return s;
}

ReturnCode domain_configure_kind(
    const Expr* expr,
    DomainConfig* domain) {
  for (; expr; expr = expr_next(expr)) {
    if (expr_is_value(expr, "linear")) {
      domain->kind = DomainKind::LINEAR;
      continue;
    }

    if (expr_is_value(expr, "log") ||
        expr_is_value(expr, "logarithmic")) {
      domain->kind = DomainKind::LOGARITHMIC;
      continue;
    }

    if (expr_is_value(expr, "invert") ||
        expr_is_value(expr, "inverted")) {
      domain->inverted = true;
      continue;
    }

    return err_invalid_value(expr_inspect(expr), {
      "linear",
      "log",
      "logarithmic",
      "inverted",
      "inverted"
    });
  }

  return OK;
}

ReturnCode scale_layout_linear(
    const DomainConfig& domain,
    ScaleLayout* layout,
    double step,
    std::optional<double> align) {
  layout->ticks.clear();
  layout->labels.clear();

  auto begin = std::max(align.value_or(domain_min(domain)), domain_min(domain));
  auto end = domain_max(domain);

  if (((end - begin) / step) > kMaxTicks) {
    return {ERROR, "too many ticks"};
  }

  size_t label_idx = 0;
  for (auto v = begin; v <= end; v += step) {
    auto vp = domain_translate(domain, v);
    layout->ticks.emplace_back(vp);
    layout->labels.emplace_back(vp);
  }

  return OK;
}

ReturnCode scale_layout_subdivide(
    const DomainConfig& domain,
    ScaleLayout* layout,
    uint32_t divisions) {
  layout->ticks.clear();
  layout->labels.clear();

  for (size_t i = 0; i < divisions; ++i) {
    auto o = (1.0f / (divisions - 1)) * i;
    layout->ticks.emplace_back(o);
    layout->labels.emplace_back(o);
  }

  return OK;
}

ReturnCode scale_layout_discrete(
    const DomainConfig& domain,
    ScaleLayout* layout) {
  uint32_t step = 1;
  uint32_t range = domain_max(domain) - domain_min(domain);

  layout->labels.clear();
  layout->ticks.clear();

  for (size_t i = 0; i <= range; i += step) {
    auto o = domain_translate(domain, i * step);
    auto o1 = domain_translate(domain, i * step - step * 0.5);
    auto o2 = domain_translate(domain, i * step + step * 0.5);
    auto v = uint32_t(domain_min(domain)) + i * step;
    auto vn = uint32_t(domain_min(domain)) + (i + 1) * step;

    if (o1 >= 0 && o2 <= 1) {
      layout->labels.emplace_back(o);
    }

    if (o1 >= 0 && o1 <= 1) {
      layout->ticks.push_back(o1);
    }

    if (o2 >= 0 && o2 <= 1) {
      layout->ticks.push_back(o2);
    }
  }

  return OK;
}

ReturnCode scale_configure_layout_linear(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  double step = 0;
  std::optional<double> align;
  switch (args.size()) {
    case 2:
      if (auto rc = expr_to_float64_opt(args[1], &align); !rc) {
        return rc;
      }
    case 1:
      if (auto rc = expr_to_float64(args[0], &step); !rc) {
        return rc;
      }
      break;
    case 0:
      step = 1; // TODO: automatically choose a good value
      break;
    default:
      return ReturnCode::error(
          "EARG",
          "invalid number of arguments for 'linear'; expected zero, one or two");
  }

  *layout = bind(
      &scale_layout_linear,
      _1,
      _2,
      step,
      align);

  return OK;
}

ReturnCode scale_configure_layout_subdivide(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  double subdivisions = 0;
  switch (args.size()) {
    case 0:
      subdivisions = 8; // TODO: automatically choose a good value
      break;
    case 1:
      if (auto rc = expr_to_float64(args[0], &subdivisions); !rc) {
        return rc;
      }
      break;
    default:
      return ReturnCode::error(
          "EARG",
          "invalid number of arguments for 'subdivide'; expected one or two");
  }

  *layout = bind(
      &scale_layout_subdivide,
      _1,
      _2,
      subdivisions);

  return OK;
}

ReturnCode scale_configure_layout(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  if (!expr || !expr_is_list(expr)) {
    return ReturnCode::errorf(
        "EARG",
        "invalid argument; expected a list but got: $0",
        "..."); // FIXME
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "linear")) {
    return scale_configure_layout_linear(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "subdivide")) {
    return scale_configure_layout_subdivide(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "discrete")) {
    *layout = bind(&scale_layout_discrete, _1, _2);
    return OK;
  }

  return ReturnCode::error(
      "EARG",
      "invalid argument; expected one of: \n"
      "  - linear\n"
      "  - subdivide\n"
      "  - discrete\n");
}


} // namespace fviz

