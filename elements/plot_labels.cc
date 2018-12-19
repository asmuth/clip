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

static const double kDefaultLabelPaddingEM = 0.8;

struct PlotLabelsConfig {
  std::vector<double> x;
  std::vector<double> y;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Measure label_font_size;
  Color label_color;
};

ReturnCode draw_labels(
    const Document& doc,
    const PlotLabelsConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& label_text = config.labels[i];
    auto label_padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingEM, config.label_font_size));

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

ReturnCode configure(
    const Document& doc,
    const plist::PropertyList& plist,
    const DomainMap& scales,
    ElementRef* elem) {
  SeriesRef data_x;
  SeriesRef data_y;
  SeriesRef data_labels;

  std::string scale_x = SCALE_DEFAULT_X;
  std::string scale_y = SCALE_DEFAULT_Y;

  PlotLabelsConfig config;
  config.label_font = doc.font_sans;
  config.label_font_size = doc.font_size;

  static const ParserDefinitions pdefs = {
    {"x", configure_series_var(&data_x)},
    {"x-scale", std::bind(&configure_string, std::placeholders::_1, &scale_x)},
    {"y", configure_series_var(&data_y)},
    {"y-scale", std::bind(&configure_string, std::placeholders::_1, &scale_y)},
    {"label", configure_series_var(&data_labels)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check dataset */
  if (!data_x || !data_y || !data_labels) {
    return ERROR_INVALID_ARGUMENT;
  }

  if ((data_x->size() != data_y->size()) ||
      (data_x->size() != data_labels->size())) {
    return ERROR_INVALID_ARGUMENT;
  }

  /* fetch domains */
  auto domain_x = domain_find(scales, scale_x);
  if (!domain_x) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_x);
  }

  auto domain_y = domain_find(scales, scale_y);
  if (!domain_y) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_y);
  }

  /* load data */
  config.x = domain_translate(*domain_x, *data_x);
  config.y = domain_translate(*domain_y, *data_y);
  config.labels = *data_labels;

  /* return element */
  auto e = std::make_unique<Element>();
  e->draw = std::bind(
      &draw_labels,
      std::placeholders::_1,
      config,
      std::placeholders::_2,
      std::placeholders::_3);

  *elem = std::move(e);
  return OK;
}

} // namespace labels
} // namespace plot
} // namespace plotfx

