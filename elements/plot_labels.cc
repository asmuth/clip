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

PlotLabelsConfig::PlotLabelsConfig() {}

ReturnCode draw_labels(
    const PlotConfig& plot,
    const PlotLabelsConfig& config,
    const Document& doc,
    const Rectangle& clip,
    Layer* layer) {
  auto dim_x = dimension_find(plot.dimensions, plot.column_x);
  if (!dim_x) {
    return ReturnCode::errorf("EARG", "dimension not found: $0", plot.column_x);
  }

  auto dim_y = dimension_find(plot.dimensions, plot.column_y);
  if (!dim_y) {
    return ReturnCode::errorf("EARG", "dimension not found: $0", plot.column_y);
  }

  const DataColumn* column_x = nullptr;
  if (auto rc = column_find(plot.data, dim_x->key, &column_x); !rc) {
    return rc;
  }

  const DataColumn* column_y = nullptr;
  if (auto rc = column_find(plot.data, dim_y->key, &column_y); !rc) {
    return rc;
  }

  if (column_x->data.size() != column_y->data.size()) {
    // FIXME error msg
    return ERROR_INVALID_ARGUMENT;
  }

  ///* draw labels */
  //for (size_t i = 0; i < column_x->data.size(); ++i) {
  //  if (i >= config.labels.size()) {
  //    break;
  //  }

  //  const auto& label_text = config.labels[i];
  //  auto label_padding = measure_or(
  //      config.label_padding,
  //      from_em(kDefaultLabelPaddingEM, config.label_font_size));

  //  Point p(
  //      clip.x + x[i] * clip.w,
  //      clip.y + (1.0 - y[i]) * clip.h - label_padding);

  //  TextStyle style;
  //  style.font = config.label_font;
  //  style.color = config.label_color;
  //  style.font_size = config.label_font_size;

  //  auto ax = HAlign::CENTER;
  //  auto ay = VAlign::BOTTOM;
  //  if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
  //    return rc;
  //  }
  //}

  return OK;
}

ReturnCode configure(const plist::Property& prop, const Document& doc, PlotConfig* config) {
  if (!plist::is_map(prop)) {
    return ERROR_INVALID_ARGUMENT;
  }

  PlotLabelsConfig layer;
  layer.label_font = doc.font_sans; // FIXME
  layer.label_font_size = doc.font_size; // FIXME

  static const ParserDefinitions pdefs = {
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

