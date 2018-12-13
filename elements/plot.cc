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
#include "plot_labels.h"
#include "plot_lines.h"
#include "plot_points.h"
#include "legend.h"

namespace plotfx {
namespace plot {

PlotConfig::PlotConfig() {
  {
    DomainConfig d;
    scales.emplace("x", d);
  }

  {
    DomainConfig d;
    d.padding = 0.1f;
    d.min_auto_snap_zero = true;
    scales.emplace("y", d);
  }
}

ReturnCode draw(
    const PlotConfig& config,
    const Document& doc,
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
  for (const auto& s : config.layers) {
    if (auto rc = s.draw(config, doc, bbox, layer); !rc) {
      return rc;
    }
  }

  // render legend
  if (auto rc = legend_draw(config.legends, bbox, layer); !rc) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode configure_layer(const plist::Property& prop, const Document& doc, PlotConfig* config) {
  std::string type = "points";
  static const ParserDefinitions pdefs = {
    {"type", std::bind(&configure_string, std::placeholders::_1, &type)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  // FIXME proper lookup
  if (type == "labels") return labels::configure(prop, doc, config);
  if (type == "lines") return lines::configure(prop, doc, config);
  if (type == "points") return points::configure(prop, doc, config);

  return ReturnCode::errorf("EARG", "invalid layer type: '$0'", type);
}

ReturnCode configure(
    const Document& doc,
    const plist::PropertyList& plist,
    ElementRef* elem) {
  PlotConfig config;

  // FIXME
  config.axis_top.font = doc.font_sans;
  config.axis_top.label_font_size = doc.font_size;
  config.axis_top.border_color = doc.border_color;
  config.axis_top.text_color = doc.text_color;
  config.axis_right.font = doc.font_sans;
  config.axis_right.label_font_size = doc.font_size;
  config.axis_right.border_color = doc.border_color;
  config.axis_right.text_color = doc.text_color;
  config.axis_bottom.font = doc.font_sans;
  config.axis_bottom.label_font_size = doc.font_size;
  config.axis_bottom.border_color = doc.border_color;
  config.axis_bottom.text_color = doc.text_color;
  config.axis_left.font = doc.font_sans;
  config.axis_left.label_font_size = doc.font_size;
  config.axis_left.border_color = doc.border_color;
  config.axis_left.text_color = doc.text_color;

  config.margins[0] = from_em(1.0, doc.font_size);
  config.margins[1] = from_em(1.0, doc.font_size);
  config.margins[2] = from_em(1.0, doc.font_size);
  config.margins[3] = from_em(1.0, doc.font_size);

  auto domain_x = domain_find(&config.scales, "x");
  auto domain_y = domain_find(&config.scales, "y");

  static const ParserDefinitions pdefs = {
    {"data", std::bind(&configure_data_frame, std::placeholders::_1, &config.data)},
    {"group", configure_key(&config.default_group_key)},
    {"x", configure_key(&config.default_x_key)},
    {"y", configure_key(&config.default_y_key)},
    {"axis-x-type", std::bind(&domain_configure, std::placeholders::_1, domain_x)},
    {"axis-x-min", std::bind(&configure_float_opt, std::placeholders::_1, &domain_x->min)},
    {"axis-x-max", std::bind(&configure_float_opt, std::placeholders::_1, &domain_x->max)},
    {
      "axis-x-format",
      configure_multiprop({
          std::bind(&confgure_format, std::placeholders::_1, &config.axis_top.label_formatter),
          std::bind(&confgure_format, std::placeholders::_1, &config.axis_bottom.label_formatter),
      })
    },
    {
      "axis-x-label-placement",
      configure_multiprop({
          std::bind(&axis_configure_label_placement, std::placeholders::_1, &config.axis_top.label_placement),
          std::bind(&axis_configure_label_placement, std::placeholders::_1, &config.axis_bottom.label_placement),
      })
    },
    {"axis-y-type", std::bind(&domain_configure, std::placeholders::_1, domain_y)},
    {"axis-y-min", std::bind(&configure_float_opt, std::placeholders::_1, &domain_y->min)},
    {"axis-y-max", std::bind(&configure_float_opt, std::placeholders::_1, &domain_y->max)},
    {
      "axis-y-format",
      configure_multiprop({
          std::bind(&confgure_format, std::placeholders::_1, &config.axis_left.label_formatter),
          std::bind(&confgure_format, std::placeholders::_1, &config.axis_right.label_formatter),
      })
    },
    {
      "axis-y-label-placement",
      configure_multiprop({
          std::bind(&axis_configure_label_placement, std::placeholders::_1, &config.axis_left.label_placement),
          std::bind(&axis_configure_label_placement, std::placeholders::_1, &config.axis_right.label_placement),
      })
    },
    {"axis-top", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_top.mode)},
    {"axis-top-format", std::bind(&confgure_format, std::placeholders::_1, &config.axis_top.label_formatter)},
    {
      "axis-top-label-placement",
      std::bind(
          &axis_configure_label_placement,
          std::placeholders::_1,
          &config.axis_top.label_placement),
    },
    {"axis-right", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_right.mode)},
    {"axis-right-format", std::bind(&confgure_format, std::placeholders::_1, &config.axis_right.label_formatter)},
    {
      "axis-right-label-placement",
      std::bind(
          &axis_configure_label_placement,
          std::placeholders::_1,
          &config.axis_right.label_placement),
    },
    {"axis-bottom", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_bottom.mode)},
    {"axis-bottom-format", std::bind(&confgure_format, std::placeholders::_1, &config.axis_bottom.label_formatter)},
    {
      "axis-bottom-label-placement",
      std::bind(
          &axis_configure_label_placement,
          std::placeholders::_1,
          &config.axis_bottom.label_placement),
    },
    {"axis-left", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_left.mode)},
    {"axis-left-format", std::bind(&confgure_format, std::placeholders::_1, &config.axis_left.label_formatter)},
    {
      "axis-left-label-placement",
      std::bind(
          &axis_configure_label_placement,
          std::placeholders::_1,
          &config.axis_left.label_placement),
    },
    {
      "margin",
      configure_multiprop({
          std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[0]),
          std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[1]),
          std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[2]),
          std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[3])
      })
    },
    {"margin-top", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[0])},
    {"margin-right", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[1])},
    {"margin-bottom", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[2])},
    {"margin-left", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &config.margins[3])},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  /* configure legend */
  if (auto rc = legend_configure_all(doc, plist, &config.legends); !rc) {
    return rc;
  }

  static const ParserDefinitions pdefs_layer = {
    {"layer", std::bind(&configure_layer, std::placeholders::_1, doc, &config)}
  };

  if (auto rc = parseAll(plist, pdefs_layer); !rc.isSuccess()) {
    return rc;
  }

  /* resolve axes */
  if (auto rc = axis_resolve(
        config.scales,
        &config.axis_top,
        &config.axis_right,
        &config.axis_bottom,
        &config.axis_left);
        !rc) {
    return rc;
  }

  auto e = std::make_unique<Element>();
  e->draw = std::bind(
      &draw,
      config,
      std::placeholders::_1,
      std::placeholders::_2,
      std::placeholders::_3);

  *elem = std::move(e);

  return ReturnCode::success();
}

} // namespace plot
} // namespace plotfx

