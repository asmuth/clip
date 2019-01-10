/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <numeric>
#include "plot_bars.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include <source/utils/algo.h>
#include "source/config_helpers.h"

using namespace std::placeholders;

namespace plotfx {
namespace plot {
namespace bars {

static const double kDefaultBarPadding = 0.44;
static const double kDefaultBarGroupPadding = 0.24;
static const double kDefaultLabelPaddingHorizEM = 0.4;
static const double kDefaultLabelPaddingVertEM = 0.4;

PlotBarsConfig::PlotBarsConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode draw_horizontal(
    const PlotBarsConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  assert(config.x.size() == config.xoffset.size());
  assert(config.x.size() == config.y.size());
  assert(config.y.size() == config.yoffset.size());

  const double group_cnt = config.groups.size();
  const double groups_cnt = (config.x.size() / group_cnt);

  const double slot_width =
      (clip.h / groups_cnt) *
      (1.0 - kDefaultBarPadding);

  const double bar_width =
      (slot_width / group_cnt) *
      (group_cnt > 1 ? (1.0 - kDefaultBarGroupPadding) : 1.0);

  for (size_t group_idx = 0; group_idx < config.groups.size(); ++group_idx) {
    const auto group = config.groups[group_idx];

    for (auto i : group.index) {
      auto sx1 = clip.x + config.x[i] * clip.w;
      auto sx2 = clip.x + config.xoffset[i] * clip.w;
      const double sy =
          clip.y + (1.0 - config.y[i]) * clip.h +
          slot_width * -.5 +
          (slot_width / group_cnt) * (group_idx + .5);

      const auto& color = config.colors.empty()
          ? Color{}
          : config.colors[i % config.colors.size()];

      auto size = config.sizes.empty()
          ? Measure(0)
          : config.sizes[i % config.sizes.size()];

      FillStyle style;
      style.color = color;

      Path path;
      path.moveTo(sx1, sy - bar_width * 0.5);
      path.lineTo(sx2, sy - bar_width * 0.5);
      path.lineTo(sx2, sy + bar_width * 0.5);
      path.lineTo(sx1, sy + bar_width * 0.5);
      path.closePath();

      fillPath(layer, clip, path, style);
    }
  }

  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& label_text = config.labels[i];

    auto size = config.sizes.empty()
        ? 0
        : config.sizes[i % config.sizes.size()].value;

    auto label_padding = size + measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingHorizEM, config.label_font_size));

    Point p(
        clip.x + config.x[i] * clip.w + label_padding,
        clip.y + (1.0 - config.y[i]) * clip.h);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::LEFT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw_vertical(
    const PlotBarsConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  assert(config.x.size() == config.xoffset.size());
  assert(config.x.size() == config.y.size());
  assert(config.y.size() == config.yoffset.size());

  const double group_cnt = config.groups.size();
  const double groups_cnt = (config.x.size() / group_cnt);

  // TODO: make configurable
  const double slot_width =
      (clip.w / groups_cnt) *
      (1.0 - kDefaultBarPadding);

  const double bar_width =
      (slot_width / group_cnt) *
      (group_cnt > 1 ? (1.0 - kDefaultBarGroupPadding) : 1.0);

  for (size_t group_idx = 0; group_idx < config.groups.size(); ++group_idx) {
    const auto group = config.groups[group_idx];

    for (auto i : group.index) {
      const double sy1 = clip.y + (1.0 - config.y[i]) * clip.h;
      const double sy2 = clip.y + (1.0 - config.yoffset[i]) * clip.h;
      const double sx =
          clip.x + config.x[i] * clip.w +
          slot_width * -.5 +
          (slot_width / group_cnt) * (group_idx + .5);

      const auto& color = config.colors.empty()
          ? Color{}
          : config.colors[i % config.colors.size()];

      auto size = config.sizes.empty()
          ? Measure(0)
          : config.sizes[i % config.sizes.size()];

      FillStyle style;
      style.color = color;

      Path path;
      path.moveTo(sx - bar_width * 0.5, sy1);
      path.lineTo(sx - bar_width * 0.5, sy2);
      path.lineTo(sx + bar_width * 0.5, sy2);
      path.lineTo(sx + bar_width * 0.5, sy1);
      path.closePath();

      fillPath(layer, clip, path, style);
    }
  }

  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& label_text = config.labels[i];

    auto size = config.sizes.empty()
        ? 0
        : config.sizes[i % config.sizes.size()].value;

    auto label_padding = size + measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingVertEM, config.label_font_size));

    Point p(
        clip.x + config.x[i] * clip.w,
        clip.y + (1.0 - config.y[i]) * clip.h - label_padding);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw(
    const PlotBarsConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  switch (config.direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(config, clip, layer);
    case Direction::VERTICAL:
      return draw_vertical(config, clip, layer);
    default:
      return ERROR;
  }
}

ReturnCode build_legend(
    const PlotBarsConfig& config,
    const std::string& title,
    const std::string& legend_key,
    LegendItemMap* legend) {
  LegendItemGroup legend_items;

  for (const auto& g : config.groups) {
    LegendItem li;
    li.title = g.key.empty() ? title : g.key;
    li.color = config.colors.empty()
        ? Color{}
        : config.colors[g.index[0] % config.colors.size()];

    legend_items.items.emplace_back(li);
  }

  legend_items_add(legend_key, legend_items, legend);
  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data,
    const Document& doc,
    const DomainMap& scales,
    LegendItemMap* legend,
    PlotBarsConfig* config) {
  SeriesRef data_x = find_maybe(data.defaults, "x");
  SeriesRef data_xoffset;
  SeriesRef data_y = find_maybe(data.defaults, "y");
  SeriesRef data_yoffset;
  SeriesRef data_group = find_maybe(data.defaults, "group");
  SeriesRef data_labels;

  std::string scale_x = SCALE_DEFAULT_X;
  std::string scale_y = SCALE_DEFAULT_Y;

  std::string title;

  std::string legend_key = LEGEND_DEFAULT;

  Direction direction = Direction::VERTICAL;

  std::optional<Color> color;
  SeriesRef colors = find_maybe(data.defaults, "colors");
  DomainConfig color_domain;
  ColorScheme color_palette;

  static const ParserDefinitions pdefs = {
    {"x", configure_series_fn(data, &data_x)},
    {"x-offset", configure_series_fn(data, &data_xoffset)},
    {"scale-x", bind(&configure_string, _1, &scale_x)},
    {"y", configure_series_fn(data, &data_y)},
    {"y-offset", configure_series_fn(data, &data_yoffset)},
    {"scale-y", bind(&configure_string, _1, &scale_y)},
    {"direction", bind(&configure_direction, _1, &direction)},
    {"group", configure_series_fn(data, &data_group)},
    {"color", configure_color_opt(&color)},
    {"colors", configure_series_fn(data, &colors)},
    {"labels", configure_series_fn(data, &data_labels)},
    {"title", bind(&configure_string, _1, &title)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check dataset */
  if (!data_x || !data_y) {
    return ReturnCode::error("EARG", "the following properties are required: x, y");
  }

  if ((data_x->size() != data_y->size()) ||
      (data_xoffset && data_x->size() != data_xoffset->size()) ||
      (data_yoffset && data_x->size() != data_yoffset->size()) ||
      (data_group && data_x->size() != data_group->size())) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'x', 'y', 'y', 'yoffset' and 'group' properties must be equal");
  }

  if (data_labels && (data_x->size() != data_labels->size())) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'x', 'y' and 'labels' properties must be equal");
  }

  /* fetch domains */
  auto domain_x = find_ptr(scales, scale_x);
  if (!domain_x) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_x);
  }

  auto domain_y = find_ptr(scales, scale_y);
  if (!domain_y) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_y);
  }

  /* group data */
  if (data_group) {
    if (data_x->size() != data_group->size()) {
      return ERROR;
    }

    config->groups = plotfx::series_group(*data_group);
  } else {
    DataGroup g;
    g.index = std::vector<size_t>(data_x->size());
    std::iota(g.index.begin(), g.index.end(), 0);
    config->groups.emplace_back(g);
  }

  /* return element */
  config->direction = direction;

  config->x = domain_translate(*domain_x, *data_x);
  config->xoffset = domain_translate(
      *domain_x,
      data_xoffset
          ? *data_xoffset
          : std::vector<Value>(data_x->size(), "0.0"));

  config->y = domain_translate(*domain_y, *data_y);
  config->yoffset = domain_translate(
      *domain_y,
      data_yoffset
          ? *data_yoffset
          : std::vector<Value>(data_y->size(), "0.0"));

  config->colors = fallback(
      color,
      series_to_colors(colors, color_domain, color_palette),
      groups_to_colors(data_x->size(), config->groups, color_palette));

  config->label_font = doc.font_sans;
  config->label_font_size = doc.font_size;
  if (data_labels) {
    config->labels = *data_labels;
  }

  /* build legend items */
  if (auto rc = build_legend(*config, title, legend_key, legend); !rc) {
    return rc;
  }

  return OK;
}

} // namespace bars
} // namespace plot
} // namespace plotfx

