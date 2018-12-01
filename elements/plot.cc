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
#include "plot_lines.h"

namespace plotfx {
namespace plot {

PlotConfig::PlotConfig() :
    margins({
        Measure(Unit::REM, 4.0f),
        Measure(Unit::REM, 5.0f),
        Measure(Unit::REM, 4.0f),
        Measure(Unit::REM, 5.0f)}) {
  domain_y.padding = 0.1f;
}

ReturnCode draw(
    const PlotConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  // setup domains
  auto domain_x = config.domain_x;
  auto domain_y = config.domain_y;

  // setup layout
  auto border_box = layout_margin_box(
      clip,
      to_unit(layer->measures, config.margins[0]).value,
      to_unit(layer->measures, config.margins[1]).value,
      to_unit(layer->measures, config.margins[2]).value,
      to_unit(layer->measures, config.margins[3]).value);

  // render axes
  if (auto rc = axis_draw_all(
        border_box,
        domain_x,
        domain_y,
        config.axis_top,
        config.axis_right,
        config.axis_bottom,
        config.axis_left,
        layer);
        !rc) {
    return rc;
  }

  // render series
  for (const auto& s : config.series) {
    if (auto rc = s.draw(config, border_box, layer); !rc) {
      return rc;
    }
  }

  return ReturnCode::success();
}

ReturnCode configure_series(const plist::Property& prop, PlotConfig* config) {
  if (auto rc = lines::configure(prop, config); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode configure(const plist::PropertyList& plist, ElementRef* elem) {
  PlotConfig config;
  static const ParserDefinitions pdefs = {
    {
      "margin",
      configure_multiprop({
          std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[0]),
          std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[1]),
          std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[2]),
          std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[3])
      })
    },
    {"margin-top", std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[0])},
    {"margin-right", std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[1])},
    {"margin-bottom", std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[2])},
    {"margin-left", std::bind(&parseMeasureProp, std::placeholders::_1, &config.margins[3])},
    {"axis-top", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_top.mode)},
    {"axis-right", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_right.mode)},
    {"axis-bottom", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_bottom.mode)},
    {"axis-left", std::bind(&parseAxisModeProp, std::placeholders::_1, &config.axis_left.mode)},
    {"xdomain-padding", std::bind(&configure_float, std::placeholders::_1, &config.domain_x.padding)},
    {"ydomain-padding", std::bind(&configure_float, std::placeholders::_1, &config.domain_y.padding)},
    {"xmin", std::bind(&configure_float_opt, std::placeholders::_1, &config.domain_x.min)},
    {"xmax", std::bind(&configure_float_opt, std::placeholders::_1, &config.domain_x.max)},
    {"ymin", std::bind(&configure_float_opt, std::placeholders::_1, &config.domain_y.min)},
    {"ymax", std::bind(&configure_float_opt, std::placeholders::_1, &config.domain_y.max)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  static const ParserDefinitions pdefs_series = {
    {"series", std::bind(&configure_series, std::placeholders::_1, &config)}
  };

  if (auto rc = parseAll(plist, pdefs_series); !rc.isSuccess()) {
    return rc;
  }

  auto e = std::make_unique<Element>();
  e->draw = std::bind(&draw, config, std::placeholders::_1, std::placeholders::_2);
  *elem = std::move(e);

  return ReturnCode::success();
}

} // namespace plot
} // namespace plotfx

