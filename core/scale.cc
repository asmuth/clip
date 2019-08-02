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
#include "data.h"
#include "core/scale.h"
#include "core/sexpr_conv.h"
#include "core/sexpr_util.h"

#include <assert.h>
#include <iostream>
#include <functional>

using namespace std::placeholders;
using std::bind;

namespace fviz {

static const double kDefaultLogBase = 10;
static const size_t kMaxTicks = 8192;
static const double EPSILON = 0.001;

ScaleConfig::ScaleConfig() :
    kind(ScaleKind::LINEAR),
    log_base(kDefaultLogBase),
    inverted(false),
    padding(0),
    limit_hints(std::make_shared<ScaleLimitHints>()) {}

void scale_fit(double value, ScaleConfig* domain) {
  if (!domain->limit_hints->min_value || *domain->limit_hints->min_value > value) {
    domain->limit_hints->min_value = std::optional<double>(value);
  }
  if (!domain->limit_hints->max_value || *domain->limit_hints->max_value < value) {
    domain->limit_hints->max_value = std::optional<double>(value);
  }
}

double scale_min(const ScaleConfig& domain) {
  auto min_auto = 0.0f;
  if (domain.limit_hints->min_value) {
    min_auto = *domain.limit_hints->min_value - domain.padding;
  }

  return domain.min.value_or(min_auto);
}

double scale_max(const ScaleConfig& domain) {
  auto max_auto = 1.0f;
  if (domain.limit_hints->max_value) {
    max_auto = *domain.limit_hints->max_value + domain.padding;
  }

  return domain.max.value_or(max_auto);
}

double scale_translate_linear(
    const ScaleConfig& domain,
    double v) {
  double min = scale_min(domain);
  double max = scale_max(domain);

  auto vt = (v - min) / (max - min);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return vt;
}

double scale_translate_log(
    const ScaleConfig& domain,
    double v) {
  if (v <= 0) {
    return  0;
  }

  auto min = scale_min(domain);
  auto max = scale_max(domain);
  auto log_base = domain.log_base;

  double min_log = min == 0 ? 0 : log(min) / log(log_base);
  double max_log = max == 0 ? 0 : log(max) / log(log_base);

  auto vf = v;
  if (vf != 0.0) {
    vf = log(vf) / log(log_base);
  } else {
    vf = 0;
  }

  auto vt = (vf - min_log) / (max_log - min_log);
  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return vt;
}

double scale_translate_categorical(
    const ScaleConfig& scale,
    size_t index) {
  double n = scale.categories.size();
  double i = std::clamp(double(index), 0.0, n - 1);
  double p = scale.padding;
  return (i + p) / (n - 1 + p * 2);
}

double scale_translate(
    const ScaleConfig& domain,
    double value) {
  switch (domain.kind) {
    case ScaleKind::LINEAR:
      return scale_translate_linear(domain, value);
    case ScaleKind::LOGARITHMIC:
      return scale_translate_log(domain, value);
    case ScaleKind::CATEGORICAL:
      return scale_translate_categorical(domain, value);
    default:
      return std::numeric_limits<double>::quiet_NaN();
  }

  return 0.0f;
}

double scale_translate_magnitude(
    const ScaleConfig& domain,
    double value) {
  switch (domain.kind) {
    case ScaleKind::LINEAR:
    case ScaleKind::LOGARITHMIC: {
      double min = scale_min(domain);
      double max = scale_max(domain);
      return value / (max - min);
    }
    case ScaleKind::CATEGORICAL:
      return 1;
    default:
      return std::numeric_limits<double>::quiet_NaN();
  }

  return 0.0f;
}

std::function<double (double)> scale_translate_fn(const ScaleConfig& domain) {
  return bind(&scale_translate, domain, std::placeholders::_1);
}

std::function<double (double)> scale_translate_magnitude_fn(const ScaleConfig& domain) {
  return bind(&scale_translate_magnitude, domain, std::placeholders::_1);
}

double  scale_untranslate_linear(const ScaleConfig& domain, double vt) {
  auto min = scale_min(domain);
  auto max = scale_max(domain);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return min + (max - min) * vt;
}

double scale_untranslate_log(const ScaleConfig& domain, double vt) {
  auto min = scale_min(domain);
  auto max = scale_max(domain);
  auto log_base = domain.log_base;
  double range = max - min;
  double range_log = log(range) / log(log_base);

  if (domain.inverted) {
    vt = 1.0 - vt;
  }

  return min + pow(log_base, vt * range_log);
}

double scale_untranslate(
    const ScaleConfig& domain,
    double value) {
  switch (domain.kind) {
    case ScaleKind::LINEAR:
      return scale_untranslate_linear(domain, value);
    case ScaleKind::LOGARITHMIC:
      return scale_untranslate_log(domain, value);
  }

  return {};
}

std::vector<double> scale_untranslate(
    const ScaleConfig& domain,
    const std::vector<double>& values) {
  std::vector<double> s;
  for (const auto& v : values) {
    s.emplace_back(scale_untranslate(domain, v));
  }

  return s;
}

ReturnCode scale_configure_kind(
    const Expr* expr,
    ScaleConfig* domain) {
  if (expr && expr_is_list(expr)) {
    expr = expr_get_list(expr);
  } else {
    return errorf(
        ERROR,
        "invalid argument; expected a list but got: {}",
        expr_inspect(expr)); // FIXME
  }

  for (; expr; expr = expr_next(expr)) {
    if (expr_is_value(expr, "linear")) {
      domain->kind = ScaleKind::LINEAR;
      continue;
    }

    if (expr_is_value(expr, "log") ||
        expr_is_value(expr, "logarithmic")) {
      domain->kind = ScaleKind::LOGARITHMIC;

      if (expr_next(expr)) {
        if (auto rc = expr_to_float64(expr_next(expr), &domain->log_base); !rc) {
          return rc;
        }

        expr = expr_next(expr);
      }

      continue;
    }

    if (expr_is_value(expr, "invert") ||
        expr_is_value(expr, "inverted")) {
      domain->inverted = true;
      continue;
    }

    if (expr_is_value(expr, "categorical")) {
      domain->kind = ScaleKind::CATEGORICAL;
      domain->padding = 0.5;

      expr = expr_next(expr);
      if (auto rc = data_load_strings(expr, &domain->categories); !rc) {
        return rc;
      }

      for (size_t i = 0; i < domain->categories.size(); ++i) {
        domain->categories_map[domain->categories[i]] = i;
      }

      continue;
    }

    return err_invalid_value(expr_inspect(expr), {
      "linear",
      "log",
      "logarithmic",
      "invert",
      "inverted",
      "categorical"
    });
  }

  return OK;
}

ReturnCode scale_layout_linear_interval(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step,
    double o_begin,
    double o_end) {
  layout->positions.clear();
  layout->labels.clear();

  auto begin = std::max(o_begin, scale_min(domain));
  auto end = std::min(o_end, scale_max(domain));

  if (((end - begin) / step) > kMaxTicks) {
    return {ERROR, "too many ticks"};
  }

  size_t idx = 0;
  for (auto v = begin; v <= end; v += step) {
    auto vp = scale_translate(domain, v);
    layout->positions.emplace_back(vp);
    layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_linear_alignat(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step,
    double align) {
  auto begin = scale_min(domain);
  auto end = scale_max(domain);

  // align
  begin -= fmod(begin, step);
  begin -= fmod(align, step);

  if (((end - begin) / step) > kMaxTicks) {
    return {ERROR, "too many ticks"};
  }

  size_t idx = 0;
  for (auto v = begin; v <= end; v += step) {
    auto vp = scale_translate(domain, v);
    if (vp < 0.0) {
      continue;
    }

    layout->positions.emplace_back(vp);
    layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_linear_align(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step) {
  auto begin = scale_min(domain);
  auto end = scale_max(domain);

  // align
  begin -= fmod(begin, step);

  if (((end - begin) / step) > kMaxTicks) {
    return {ERROR, "too many ticks"};
  }

  size_t idx = 0;
  for (auto v = begin; v <= end; v += step) {
    auto vp = scale_translate(domain, v);
    if (vp < 0.0) {
      continue;
    }

    layout->positions.emplace_back(vp);
    layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_linear(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double step) {
  auto begin = scale_min(domain);
  auto end = scale_max(domain);

  if (((end - begin) / step) > kMaxTicks) {
    return {ERROR, "too many ticks"};
  }

  size_t idx = 0;
  for (auto v = begin; v <= end; v += step) {
    auto vp = scale_translate(domain, v);
    layout->positions.emplace_back(vp);
    layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_exponential_steps(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double base,
    size_t steps) {
  double min = scale_min(domain);
  double max = scale_max(domain);
  double exp = min == 0 ? 0 : log(min) / log(domain.log_base);

  for (size_t idx = 0; ; ++idx) {
    auto v = pow(base, exp++);
    auto vn = pow(base, exp);

    if (v < min) {
      continue;
    }

    if (v > max + EPSILON) {
      break;
    }

    for (size_t s = 0; s < steps - 1; ++s) {
      auto vs = v + (s / double(steps - 1)) * (vn - v);
      auto vp = scale_translate(domain, vs);
      if (vs > max + EPSILON) {
        break;
      }

      layout->positions.emplace_back(vp);
      layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
    }
  }

  return OK;
}

ReturnCode scale_layout_exponential(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    double base) {
  double min = scale_min(domain);
  double max = scale_max(domain);
  double exp = min == 0 ? 0 : log(min) / log(domain.log_base);

  size_t idx = 0;
  for (;;) {
    auto v = pow(base, exp++);
    auto vp = scale_translate(domain, v);

    if (v < min) {
      continue;
    }

    if (v > max + EPSILON) {
      break;
    }

    layout->positions.emplace_back(vp);
    layout->labels.emplace_back(label_format(idx++, std::to_string(v))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_subdivide(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout,
    uint32_t divisions) {
  layout->positions.clear();
  layout->labels.clear();

  for (size_t i = 0; i <= divisions; ++i) {
    auto o = (1.0f / divisions) * i;
    layout->positions.emplace_back(o);
    layout->labels.emplace_back(
        label_format(i, std::to_string(scale_untranslate(domain, o)))); // FIXME
  }

  return OK;
}

ReturnCode scale_layout_categorical(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout) {
  layout->positions.clear();
  layout->labels.clear();

  auto n = domain.categories.size();
  for (size_t i = 0; i < n; ++i) {
    auto o = scale_translate_categorical(domain, i);
    layout->positions.emplace_back(o);
    layout->labels.emplace_back(label_format(i, domain.categories[i]));
  }

  return OK;
}

ReturnCode scale_layout_categorical_bounds(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout) {
  layout->positions.clear();
  layout->labels.clear();

  auto n = domain.categories.size();
  double p = domain.padding;
  auto o = 0.5 / (n - 1 + p * 2);

  for (size_t i = 0; i <= n; ++i) {
    auto v = scale_translate_categorical(domain, i);
    layout->positions.emplace_back(i == n ? v + o : v - o);
    layout->labels.emplace_back("");
  }

  return OK;
}

ReturnCode scale_layout_none(
    const ScaleConfig& domain,
    const Formatter& label_format,
    ScaleLayout* layout) {
  return OK;
}

ReturnCode scale_configure_layout_linear_interval(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 3) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'linear-interval'; expected three, got: {}",
        args.size());
  }

  double step;
  if (auto rc = expr_to_float64(args[0], &step); !rc) {
    return rc;
  }

  double begin;
  if (auto rc = expr_to_float64(args[1], &begin); !rc) {
    return rc;
  }

  double end;
  if (auto rc = expr_to_float64(args[2], &end); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_linear_interval,
      _1,
      _2,
      _3,
      step,
      begin,
      end);

  return OK;
}

ReturnCode scale_configure_layout_linear_alignat(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 2) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'linear-alignat'; expected two, got: {}",
        args.size());
  }

  double step;
  if (auto rc = expr_to_float64(args[0], &step); !rc) {
    return rc;
  }

  double align;
  if (auto rc = expr_to_float64(args[1], &align); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_linear_alignat,
      _1,
      _2,
      _3,
      step,
      align);

  return OK;
}

ReturnCode scale_configure_layout_linear_align(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 1) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'linear-align'; expected one, got: {}",
        args.size());
  }

  double step;
  if (auto rc = expr_to_float64(args[0], &step); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_linear_align,
      _1,
      _2,
      _3,
      step);

  return OK;
}

ReturnCode scale_configure_layout_linear(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 1) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'linear'; expected one, got: {}",
        args.size());
  }

  double step;
  if (auto rc = expr_to_float64(args[0], &step); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_linear,
      _1,
      _2,
      _3,
      step);

  return OK;
}

ReturnCode scale_configure_layout_exponential_steps(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 2) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'exponential-steps'; expected two, got: {}",
        args.size());
  }

  double base;
  if (auto rc = expr_to_float64(args[0], &base); !rc) {
    return rc;
  }

  double steps;
  if (auto rc = expr_to_float64(args[1], &steps); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_exponential_steps,
      _1,
      _2,
      _3,
      base,
      steps);

  return OK;
}

ReturnCode scale_configure_layout_exponential(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  auto args = expr_collect(expr);

  if (args.size() != 1) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'exponential'; expected one, got: {}",
        args.size());
  }

  double base;
  if (auto rc = expr_to_float64(args[0], &base); !rc) {
    return rc;
  }

  *layout = bind(
      &scale_layout_exponential,
      _1,
      _2,
      _3,
      base);

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
      return error(
          ERROR,
          "invalid number of arguments for 'subdivide'; expected one or two");
  }

  *layout = bind(
      &scale_layout_subdivide,
      _1,
      _2,
      _3,
      subdivisions);

  return OK;
}

ReturnCode scale_configure_layout(
    const Expr* expr,
    ScaleLayoutFn* layout) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list but got: {}",
        "..."); // FIXME
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "linear")) {
    return scale_configure_layout_linear(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "linear-align")) {
    return scale_configure_layout_linear_align(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "linear-alignat")) {
    return scale_configure_layout_linear_alignat(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "linear-interval")) {
    return scale_configure_layout_linear_interval(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "exponential")) {
    return scale_configure_layout_exponential(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "exponential-steps")) {
    return scale_configure_layout_exponential_steps(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "subdivide")) {
    return scale_configure_layout_subdivide(expr_next(expr), layout);
  }

  if (expr_is_value(expr, "categorical")) {
    *layout = bind(&scale_layout_categorical, _1, _2, _3);
    return OK;
  }

  if (expr_is_value(expr, "categorical-bounds")) {
    *layout = bind(&scale_layout_categorical_bounds, _1, _2, _3);
    return OK;
  }

  if (expr_is_value(expr, "none")) {
    *layout = bind(&scale_layout_none, _1, _2, _3);
    return OK;
  }

  return error(
      ERROR,
      "invalid argument; expected one of: \n"
      "  - linear\n"
      "  - linear-align\n"
      "  - linear-alignat\n"
      "  - linear-interval\n"
      "  - exponential\n"
      "  - exponential-steps\n"
      "  - subdivide\n"
      "  - categorical\n"
      "  - categorical-bounds\n"
      "  - none\n");
}

void scale_configure_layout_defaults(
    const ScaleConfig& config,
    ScaleLayoutFn* label_placement,
    ScaleLayoutFn* tick_placement) {
  // default label placement
  if (label_placement && !*label_placement) {
    switch (config.kind) {
      case ScaleKind::CATEGORICAL:
        *label_placement = bind(&scale_layout_categorical, _1, _2, _3);
        break;
      case ScaleKind::LOGARITHMIC:
        *label_placement = bind(
            &scale_layout_exponential,
            _1,
            _2,
            _3,
            config.log_base);
        break;
      default:
        *label_placement = bind(&scale_layout_subdivide, _1, _2, _3, 10);
        break;
    }
  }

  // default tick placement
  if (tick_placement && !*tick_placement) {
    switch (config.kind) {
      case ScaleKind::CATEGORICAL:
        *tick_placement = bind(&scale_layout_categorical_bounds, _1, _2, _3);
        break;
      case ScaleKind::LOGARITHMIC:
        if (config.log_base == 10) {
          *tick_placement = bind(
              &scale_layout_exponential_steps,
              _1,
              _2,
              _3,
              10,
              10);
        } else {
          *tick_placement =  bind(
              &scale_layout_exponential,
              _1,
              _2,
              _3,
              config.log_base);
        }
        break;
      default:
        if (label_placement) {
          *tick_placement = *label_placement;
        } else {
          *tick_placement =  bind(&scale_layout_subdivide, _1, _2, _3, 10);
        }
        break;
    }
  }
}

} // namespace fviz

