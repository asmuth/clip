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

PlotLinesConfig::PlotLinesConfig() {}

ReturnCode draw_lines(
    const PlotConfig& plot,
    const PlotLinesConfig& config,
    const Document& doc,
    const Rectangle& clip,
    Layer* layer) {
  const DataColumn* column_x = nullptr;
  if (auto rc = column_find(plot.data, plot.column_x, &column_x); !rc) {
    return rc;
  }

  const DataColumn* column_y = nullptr;
  if (auto rc = column_find(plot.data, plot.column_y, &column_y); !rc) {
    return rc;
  }

  const auto& domain_x = plot.domain_x;
  const auto& domain_y = plot.domain_y;

  if (column_x->data.size() != column_y->data.size()) {
    // FIXME error msg
    return ERROR_INVALID_ARGUMENT;
  }

  auto x = domain_translate(domain_x, column_x->data);
  auto y = domain_translate(domain_y, column_y->data);

  /* draw line */
  {
    Path path;

    for (size_t i = 0; i < column_x->data.size(); ++i) {
      auto sx = clip.x + x[i] * clip.w;
      auto sy = clip.y + (1.0 - y[i]) * clip.h;

      if (i == 0) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.line_width = measure_or(
        config.line_width,
        from_pt(kDefaultLineWidthPT, doc.dpi));

    style.color = config.line_color;
    strokePath(layer, clip, path, style);
  }

  /* draw points */
  auto point_size = config.point_size;
  if (point_size > 0) {
    FillStyle style;
    style.color = config.point_color;
    for (size_t i = 0; i < column_x->data.size(); ++i) {
      auto sx = clip.x + x[i] * clip.w;
      auto sy = clip.y + (1.0 - y[i]) * clip.h;

      // FIXME point style
      Path path;
      path.moveTo(sx + point_size, sy);
      path.arcTo(sx, sy, point_size, 0, M_PI * 2);
      fillPath(layer, clip, path, style);
    }
  }

  /* draw labels */
  for (size_t i = 0; i < column_x->data.size(); ++i) {
    if (i >= config.labels.size()) {
      break;
    }

    const auto& label_text = config.labels[i];
    auto label_padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingEM, config.label_font_size));

    Point p(
        clip.x + x[i] * clip.w,
        clip.y + (1.0 - y[i]) * clip.h - label_padding);

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

ReturnCode configure(const plist::Property& prop, const Document& doc, PlotConfig* config) {
  if (!plist::is_map(prop)) {
    return ERROR_INVALID_ARGUMENT;
  }

  auto color = config->color_scheme.next();

  PlotLinesConfig series;
  series.line_color = color;
  series.point_color = color;
  series.label_font = doc.font_sans;
  series.label_font_size = doc.font_size;
  series.label_color = doc.text_color; // FIXME: lighten by 20%

  static const ParserDefinitions pdefs = {
    {"title", std::bind(&configure_string, std::placeholders::_1, &series.title)},
    {
      "color",
      configure_multiprop({
          std::bind(&configure_color, std::placeholders::_1, &series.line_color),
          std::bind(&configure_color, std::placeholders::_1, &series.point_color),
      })
    },
    {"line-color", std::bind(&configure_color, std::placeholders::_1, &series.line_color)},
    {"line-width", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &series.line_width)},
    {"point-color", std::bind(&configure_color, std::placeholders::_1, &series.point_color)},
    {"point-size", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &series.point_size)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  config->series.emplace_back(PlotSeries {
    .draw = std::bind(
        &draw_lines,
        std::placeholders::_1,
        series,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4),
  });

  config->legend.addEntry(series.title, color);
  return OK;
}

} // namespace lines
} // namespace plot
} // namespace plotfx

