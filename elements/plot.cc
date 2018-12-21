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
#include "plot.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "common/config_helpers.h"
#include "common/element_factory.h"
#include "common/utils/algo.h"
#include "plot_labels.h"
#include "plot_lines.h"
#include "plot_points.h"
#include "legend.h"

using namespace std::placeholders;
using std::ref;

namespace plotfx {
namespace plot {

ReturnCode draw(
    const PlotConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  // setup layout
  auto bbox = layout_margin_box(
      clip,
      config.margins[0],
      config.margins[1],
      config.margins[2],
      config.margins[3]);

  if (auto rc = axis_layout(
        bbox,
        config.axis_top,
        config.axis_right,
        config.axis_bottom,
        config.axis_left,
        *layer,
        &bbox); !rc) {
    return rc;
  }

  // render axes
  if (auto rc = axis_draw_all(
        bbox,
        config.axis_top,
        config.axis_right,
        config.axis_bottom,
        config.axis_left,
        layer);
        !rc) {
    return rc;
  }

  // render layer
  for (const auto& e : config.layers) {
    if (auto rc = e->draw(bbox, layer); !rc) {
      return rc;
    }
  }

  // render legend
  if (auto rc = legend_draw(config.legends, bbox, layer); !rc) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode configure_layer(
    const plist::Property& prop,
    const Document& doc,
    const DataContext& data,
    const DomainMap& scales,
    PlotConfig* config) {
  std::string type = "points";
  static const ParserDefinitions pdefs = {
    {"type", bind(&configure_string, _1, &type)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  const auto& layer_props = *prop.next;
  ElementBuilder layer_builder;

  // TODO: proper lookup
  if (type == "labels")
    layer_builder = elem_builder<labels::PlotLabelsConfig>(
        bind(&labels::configure, _1, _2, _3, scales, _4),
        &labels::draw);

  if (type == "lines")
    layer_builder = elem_builder<lines::PlotLinesConfig>(
        bind(&lines::configure, _1, _2, _3, scales, _4),
        &lines::draw);

  if (type == "points")
    layer_builder = elem_builder<points::PlotPointsConfig>(
        bind(&points::configure, _1, _2, _3, scales, _4),
        &points::draw);

  if (!layer_builder) {
    return ReturnCode::errorf("EARG", "invalid layer type: '$0'", type);
  }

  ElementRef layer;
  if (auto rc = layer_builder(layer_props, data, doc, &layer); !rc) {
    return rc;
  }

  config->layers.emplace_back(layer);
  return OK;
}

ReturnCode configure_layers(
    const plist::PropertyList& plist,
    const Document& doc,
    const DataContext& data,
    const DomainMap& scales,
    PlotConfig* config) {
  static const ParserDefinitions pdefs_layer = {
    {"layer", bind(&configure_layer, _1, doc, data, scales, config)}
  };

  return parseAll(plist, pdefs_layer);
}

ReturnCode configure_scales(
    const plist::PropertyList& plist,
    const DataContext& data,
    DomainMap* scales) {
  {
    DomainConfig d;
    scales->emplace(SCALE_DEFAULT_X, d);
  }

  {
    DomainConfig d;
    d.padding = 0.1f;
    d.min_auto_snap_zero = true;
    scales->emplace(SCALE_DEFAULT_Y, d);
  }

  for (const auto& prop : plist) {
    if (prop.name != "layer") {
      continue;
    }

    if (!plist::is_map(prop)) {
      return ERROR_INVALID_ARGUMENT;
    }

    SeriesRef data_x = find_maybe(data.defaults, "x");
    SeriesRef data_y = find_maybe(data.defaults, "y");
    std::string scale_x = SCALE_DEFAULT_X;
    std::string scale_y = SCALE_DEFAULT_Y;

    static const ParserDefinitions pdefs = {
      {"x", configure_series_fn(data, &data_x)},
      {"x-scale", bind(&configure_string, _1, &scale_x)},
      {"y", configure_series_fn(data, &data_y)},
      {"y-scale", bind(&configure_string, _1, &scale_y)},
    };

    if (auto rc = parseAll(*prop.next, pdefs); !rc) {
      return rc;
    }

    if (data_x) {
      auto domain_x = find_ptr(scales, scale_x);
      if (!domain_x) {
        return ReturnCode::errorf("EARG", "scale not found: $0", scale_x);
      }

      domain_fit(*data_x, domain_x);
    }

    if (data_y) {
      auto domain_y = find_ptr(scales, scale_y);
      if (!domain_y) {
        return ReturnCode::errorf("EARG", "scale not found: $0", scale_y);
      }

      domain_fit(*data_y, domain_y);
    }
  }

  return OK;
}

ReturnCode configure_data_refs(
    const plist::PropertyList& plist,
    DataContext* data) {
  SeriesRef data_x;
  SeriesRef data_y;
  SeriesRef data_group;

  auto rc = parseAll(plist, {
    {"x", configure_series_fn(*data, &data_x)},
    {"y", configure_series_fn(*data, &data_y)},
    {"group", configure_series_fn(*data, &data_group)},
  });

  if (!rc) {
    return rc;
  }

  /* extend data context */
  data->defaults["x"] = data_x;
  data->defaults["y"] = data_y;
  data->defaults["group"] = data_group;
  return OK;
}

ReturnCode configure_style(
    const plist::PropertyList& plist,
    const Document& doc,
    DomainMap* scales,
    PlotConfig* config) {
  // TODO: improved style configuration
  config->axis_top.font = doc.font_sans;
  config->axis_top.label_font_size = doc.font_size;
  config->axis_top.border_color = doc.border_color;
  config->axis_top.text_color = doc.text_color;
  config->axis_right.font = doc.font_sans;
  config->axis_right.label_font_size = doc.font_size;
  config->axis_right.border_color = doc.border_color;
  config->axis_right.text_color = doc.text_color;
  config->axis_bottom.font = doc.font_sans;
  config->axis_bottom.label_font_size = doc.font_size;
  config->axis_bottom.border_color = doc.border_color;
  config->axis_bottom.text_color = doc.text_color;
  config->axis_left.font = doc.font_sans;
  config->axis_left.label_font_size = doc.font_size;
  config->axis_left.border_color = doc.border_color;
  config->axis_left.text_color = doc.text_color;

  config->margins[0] = from_em(1.0, doc.font_size);
  config->margins[1] = from_em(1.0, doc.font_size);
  config->margins[2] = from_em(1.0, doc.font_size);
  config->margins[3] = from_em(1.0, doc.font_size);

  auto domain_x = find_ptr(scales, SCALE_DEFAULT_X);
  auto domain_y = find_ptr(scales, SCALE_DEFAULT_Y);

  static const ParserDefinitions pdefs = {
    {"axis-x", bind(&domain_configure, _1, domain_x)},
    {"axis-x-min", bind(&configure_float_opt, _1, &domain_x->min)},
    {"axis-x-max", bind(&configure_float_opt, _1, &domain_x->max)},
    {
      "axis-x-format",
      configure_multiprop({
          bind(&confgure_format, _1, &config->axis_top.label_formatter),
          bind(&confgure_format, _1, &config->axis_bottom.label_formatter),
      })
    },
    {
      "axis-x-label-placement",
      configure_multiprop({
          bind(&axis_configure_label_placement, _1, &config->axis_top.label_placement),
          bind(&axis_configure_label_placement, _1, &config->axis_bottom.label_placement),
      })
    },
    {"axis-y", bind(&domain_configure, _1, domain_y)},
    {"axis-y-min", bind(&configure_float_opt, _1, &domain_y->min)},
    {"axis-y-max", bind(&configure_float_opt, _1, &domain_y->max)},
    {
      "axis-y-format",
      configure_multiprop({
          bind(&confgure_format, _1, &config->axis_left.label_formatter),
          bind(&confgure_format, _1, &config->axis_right.label_formatter),
      })
    },
    {
      "axis-y-label-placement",
      configure_multiprop({
          bind(&axis_configure_label_placement, _1, &config->axis_left.label_placement),
          bind(&axis_configure_label_placement, _1, &config->axis_right.label_placement),
      })
    },
    {"axis-top", bind(&parseAxisModeProp, _1, &config->axis_top.mode)},
    {"axis-top-format", bind(&confgure_format, _1, &config->axis_top.label_formatter)},
    {
      "axis-top-label-placement",
      bind(
          &axis_configure_label_placement,
          _1,
          &config->axis_top.label_placement),
    },
    {"axis-right", bind(&parseAxisModeProp, _1, &config->axis_right.mode)},
    {"axis-right-format", bind(&confgure_format, _1, &config->axis_right.label_formatter)},
    {
      "axis-right-label-placement",
      bind(
          &axis_configure_label_placement,
          _1,
          &config->axis_right.label_placement),
    },
    {"axis-bottom", bind(&parseAxisModeProp, _1, &config->axis_bottom.mode)},
    {"axis-bottom-format", bind(&confgure_format, _1, &config->axis_bottom.label_formatter)},
    {
      "axis-bottom-label-placement",
      bind(
          &axis_configure_label_placement,
          _1,
          &config->axis_bottom.label_placement),
    },
    {"axis-left", bind(&parseAxisModeProp, _1, &config->axis_left.mode)},
    {"axis-left-format", bind(&confgure_format, _1, &config->axis_left.label_formatter)},
    {
      "axis-left-label-placement",
      bind(
          &axis_configure_label_placement,
          _1,
          &config->axis_left.label_placement),
    },
    {
      "margin",
      configure_multiprop({
          bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[0]),
          bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[1]),
          bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[2]),
          bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[3])
      })
    },
    {"margin-top", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[0])},
    {"margin-right", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[1])},
    {"margin-bottom", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[2])},
    {"margin-left", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &config->margins[3])},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data_in,
    const Document& doc,
    PlotConfig* config) {
  DataContext data = data_in;
  DomainMap scales;

  return try_chain({
    bind(&configure_datasource, ref(plist), &data),
    bind(&configure_data_refs, ref(plist), &data),
    bind(&configure_scales, ref(plist), ref(data), &scales),
    bind(&configure_style, ref(plist), doc, &scales, ref(config)),
    bind(&configure_layers, ref(plist), doc, ref(data), ref(scales), ref(config)),
    bind(&legend_configure_all, doc, ref(plist), &config->legends),
    bind(&axis_resolve,
        ref(scales),
        &config->axis_top,
        &config->axis_right,
        &config->axis_bottom,
        &config->axis_left),
  });
}

} // namespace plot
} // namespace plotfx

