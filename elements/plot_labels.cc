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
#include "plot_labels.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "common/config_helpers.h"

namespace plotfx {
namespace plot {
namespace labels {

static const double kDefaultLineWidthPT = 2;
static const double kDefaultLabelPaddingEM = 0.8;

PlotLabelsConfig::PlotLabelsConfig() :
    x_scale("x"),
    y_scale("y") {}

ReturnCode draw_labels(
    const PlotConfig& plot,
    const PlotLabelsConfig& config,
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

  const DataColumn* labels = nullptr;
  if (auto rc = column_find(plot.data, config.label_key, &labels); !rc) {
    return rc;
  }

  if (column_x->data.size() != column_y->data.size() ||
      column_x->data.size() != labels->data.size()) {
    return ReturnCode::error("EARG", "columns have differing lengths");
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

  /* draw labels */
  auto x = domain_translate(*domain_x, column_x->data);
  auto y = domain_translate(*domain_y, column_y->data);
  for (size_t i = 0; i < column_x->data.size(); ++i) {
    const auto& label_text = labels->data[i];
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

  PlotLabelsConfig layer;
  layer.x_key = config->default_x_key;
  layer.y_key = config->default_y_key;
  layer.label_key = config->default_y_key;
  layer.label_font = doc.font_sans;
  layer.label_font_size = doc.font_size;

  static const ParserDefinitions pdefs = {
    {"x", configure_key(&layer.x_key)},
    {"x-scale", std::bind(&configure_string, std::placeholders::_1, &layer.x_scale)},
    {"y", configure_key(&layer.y_key)},
    {"y-scale", std::bind(&configure_string, std::placeholders::_1, &layer.y_scale)},
    {"label", configure_key(&layer.label_key)},
  };

  if (auto rc = parseAll(*prop.next, pdefs); !rc) {
    return rc;
  }

  config->layers.emplace_back(PlotLayer {
    .draw = std::bind(
        &draw_labels,
        std::placeholders::_1,
        layer,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4),
  });

  return OK;
}

} // namespace labels
} // namespace plot
} // namespace plotfx

