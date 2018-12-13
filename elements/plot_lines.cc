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
#include "plot_lines.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "common/config_helpers.h"

namespace plotfx {
namespace plot {
namespace lines {

static const double kDefaultLineWidthPT = 2;
static const double kDefaultLabelPaddingEM = 0.8;

PlotLinesConfig::PlotLinesConfig() :
    x_scale("x"),
    y_scale("y"),
    legend_key(LEGEND_DEFAULT) {}

ReturnCode draw_lines(
    const PlotConfig& plot,
    const PlotLinesConfig& config,
    const Document& doc,
    const Rectangle& clip,
    Layer* layer) {
  /* fetch columns */
  const DataColumn* column_x = nullptr;
  if (auto rc = column_find(plot.data, config.x_key, &column_x); !rc) {
    return rc;
  }

  const DataColumn* column_y = nullptr;
  if (auto rc = column_find(plot.data, config.y_key, &column_y); !rc) {
    return rc;
  }

  if (column_x->data.size() != column_y->data.size()) {
    return ERROR_INVALID_ARGUMENT;
  }

  const DataColumn* column_group = nullptr;
  if (!config.group_key.empty()) {
    if (auto rc = column_find(plot.data, config.group_key, &column_group); !rc) {
      return rc;
    }
  }

  /* fetch domains */
  auto domain_x = domain_find(plot.scales, config.x_scale);
  if (!domain_x) {
    return ReturnCode::errorf("EARG", "scale not found: $0", config.x_scale);
  }

  auto domain_y = domain_find(plot.scales, config.y_scale);
  if (!domain_y) {
    return ReturnCode::errorf("EARG", "scale not found: $0", config.y_scale);
  }

  /* group data */
  std::vector<DataGroup> groups;
  if (column_group) {
    groups = plotfx::column_group(*column_group);
  } else {
    DataGroup g;
    g.begin = 0;
    g.end = column_x->data.size();
    groups.emplace_back(g);
  }

  /* draw lines */
  auto x = domain_translate(*domain_x, column_x->data);
  auto y = domain_translate(*domain_y, column_y->data);
  for (const auto& group : groups) {
    Color color;
    if (auto rc = resolve_slot(
          config.line_color,
          dimension_map_color_discrete(config.line_color_palette),
          plot.data,
          group.begin,
          &color); !rc) {
      return rc;
    }

    Path path;
    for (size_t i = group.begin; i < group.end; ++i) {
      auto sx = clip.x + x[i] * clip.w;
      auto sy = clip.y + (1.0 - y[i]) * clip.h;

      if (i == group.begin) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.color = color;
    style.line_width = measure_or(
        config.line_width,
        from_pt(kDefaultLineWidthPT, doc.dpi));

    strokePath(layer, clip, path, style);
  }

  return OK;
}

ReturnCode configure_legend(
    const PlotConfig& plot,
    const PlotLinesConfig& config,
    LegendConfig* legend) {
  /* fetch columns */
  const DataColumn* column_x = nullptr;
  if (auto rc = column_find(plot.data, config.x_key, &column_x); !rc) {
    return rc;
  }

  const DataColumn* column_y = nullptr;
  if (auto rc = column_find(plot.data, config.y_key, &column_y); !rc) {
    return rc;
  }

  if (column_x->data.size() != column_y->data.size()) {
    return ReturnCode::error("EARG", "columns have differing lengths");
  }

  const DataColumn* column_group = nullptr;
  if (!config.group_key.empty()) {
    if (auto rc = column_find(plot.data, config.group_key, &column_group); !rc) {
      return rc;
    }
  }

  /* group data */
  std::vector<DataGroup> groups;
  if (!column_group) {
    return OK;
  }

  /* add legend items */
  LegendGroup g;
  for (const auto& group : plotfx::column_group(*column_group)) {
    Color color;
    if (auto rc = resolve_slot(
          config.line_color,
          dimension_map_color_discrete(config.line_color_palette),
          plot.data,
          group.begin,
          &color); !rc) {
      return rc;
    }

    legend_add_item(&g, group.key, color);
  }

  legend->groups.emplace_back(g);
  return OK;
}

ReturnCode configure(
    const plist::Property& prop,
    const Document& doc,
    PlotConfig* config) {
  if (!plist::is_map(prop)) {
    return ERROR_INVALID_ARGUMENT;
  }

  PlotLinesConfig layer;
  layer.x_key = config->default_x_key;
  layer.y_key = config->default_y_key;
  layer.group_key = config->default_group_key;

  static const ParserDefinitions pdefs = {
    {"x", configure_key(&layer.x_key)},
    {"x-scale", std::bind(&configure_string, std::placeholders::_1, &layer.x_scale)},
    {"y", configure_key(&layer.y_key)},
    {"y-scale", std::bind(&configure_string, std::placeholders::_1, &layer.y_scale)},
    {"group", std::bind(&configure_string, std::placeholders::_1, &layer.group_key)},
    {"legend", std::bind(&configure_string, std::placeholders::_1, &layer.legend_key)},
    {"line-color", configure_slot(&layer.line_color)},
    {"line-width", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &layer.line_width)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  if (layer.line_color.dimension) {
    if (auto rc = dimension_resolve(config->data, &*layer.line_color.dimension); !rc) {
      return rc;
    }
  }

  domain_fit(config->data, layer.x_key, layer.x_scale, &config->scales);
  domain_fit(config->data, layer.y_key, layer.y_scale, &config->scales);

  if (auto legend = legend_find(&config->legends, layer.legend_key); legend) {
    if (auto rc = configure_legend(*config, layer, legend); !rc) {
      return rc;
    }
  }

  config->layers.emplace_back(PlotLayer {
    .draw = std::bind(
        &draw_lines,
        std::placeholders::_1,
        layer,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4),
  });

  return OK;
}

} // namespace lines
} // namespace plot
} // namespace plotfx

