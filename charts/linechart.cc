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
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include "linechart.h"
#include "common/config_helpers.h"

namespace plotfx {
namespace linechart {

/*
 *
char LineChart::kDefaultLineStyle[] = "solid";
char LineChart::kDefaultLineWidth[] = "2";
char LineChart::kDefaultPointStyle[] = "none";
char LineChart::kDefaultPointSize[] = "3";

LineChart::LineChart(
    AnyDomain* x_domain,
    AnyDomain* y_domain) :
    show_labels_(false) {
  x_domain_.reset(x_domain);
  y_domain_.reset(y_domain);
}

void LineChart::addSeries(Series2D<TX, TY>* series) {
  Domain<TX>* x_domain;
  if (x_domain_.empty()) {
    x_domain = Domain<TX>::mkDomain();
    x_domain_.reset(x_domain, true);
  } else {
    x_domain = x_domain_.getAs<Domain<TX>>();
  }

  Domain<TY>* y_domain;
  if (y_domain_.empty()) {
    y_domain = Domain<TY>::mkDomain();
    y_domain_.reset(y_domain, true);

    auto cont = dynamic_cast<AnyContinuousDomain*>(y_domain);
    if (cont != nullptr) {
      cont->setPadding(
          AnyDomain::kDefaultDomainPadding,
          AnyDomain::kDefaultDomainPadding);
    }
  } else {
    y_domain = y_domain_.getAs<Domain<TY>>();
  }

  for (const auto& point : series->getData()) {
    x_domain->addValue(point.x());
    y_domain->addValue(point.y());
  }

  series_.push_back(series);

  if (!series->hasProperty(Series::P_COLOR)) {
    color_palette_.setNextColor(series);
  }

  series->setDefaultProperty(
      Series::P_LINE_STYLE,
      LineChart::kDefaultLineStyle);

  series->setDefaultProperty(
      Series::P_LINE_WIDTH,
      LineChart::kDefaultLineWidth);

  series->setDefaultProperty(
      Series::P_POINT_STYLE,
      LineChart::kDefaultPointStyle);

  series->setDefaultProperty(
      Series::P_POINT_SIZE,
      LineChart::kDefaultPointSize);

  //Drawable::addSeries(series);
}

void LineChart::render(
    Layer* target,
    Viewport* viewport) const {
  if (x_domain_.get() == nullptr || y_domain_.get() == nullptr) {
    RAISE(kRuntimeError, "could not build domains");
  }

  x_domain_.get()->build();
  y_domain_.get()->build();


  for (const auto& series : series_) {
    Path path;

    auto point_style = series->getProperty(Series::P_POINT_STYLE);
    double point_size;
    auto line_style = series->getProperty(Series::P_LINE_STYLE);
    double line_width;

    try {
      line_width = std::stod(series->getProperty(Series::P_LINE_WIDTH));
    } catch (const std::exception& e) {
      RAISE(
          kRuntimeError,
          "invalid line width: %s",
          series->getProperty(Series::P_LINE_WIDTH).c_str());
    }

    try {
      point_size = std::stod(series->getProperty(Series::P_POINT_SIZE));
    } catch (const std::exception& e) {
      RAISE(
          kRuntimeError,
          "invalid point size: %s",
          series->getProperty(Series::P_POINT_SIZE).c_str());
    }

    auto color = series->getProperty(Series::P_COLOR);

    for (const auto& point : series->getData()) {
      auto x = x_domain_.getAs<Domain<TX>>()->scale(point.x());
      auto y = y_domain_.getAs<Domain<TY>>()->scale(point.y());
      auto ss_x = viewport->paddingLeft() + x * viewport->innerWidth();
      auto ss_y = viewport->paddingTop() + (1.0 - y) * viewport->innerHeight();
      auto label = series->labelFor(&point);

      if (show_labels_) {
        drawText(
            target,
            label,
            ss_x,
            ss_y - point_size - kLabelPadding,
            "middle",
            "text-after-edge",
            "label");
      }

      //target->drawPoint(
      //    ss_x,
      //    ss_y,
      //    point_style,
      //    point_size,
      //    color,
      //    "point",
      //    label,
      //    series->name());

      if (path.empty()) {
        path.moveTo(ss_x, ss_y);
      } else {
        path.lineTo(ss_x, ss_y);
      }
    }

    StrokeStyle style;
    style.line_width = line_width;
    //style.colour = colour;
    strokePath(
        target,
        path.data(),
        path.size(),
        style);
  }

}

AnyDomain* LineChart::getDomain(AnyDomain::kDimension dimension) {
  switch (dimension) {
    case AnyDomain::DIM_X:
      return x_domain_.get();

    case AnyDomain::DIM_Y:
      return y_domain_.get();

    case AnyDomain::DIM_Z:
      RAISE(kRuntimeError, "LineChart does not have a Z domain");
      return nullptr;
  }
}

void LineChart::setLabels(bool show_labels) {
  show_labels_ = show_labels;
}

*/

LinechartConfig::LinechartConfig() :
    x_domain(PlotDomain::LINEAR),
    y_domain(PlotDomain::LINEAR) {}

ReturnCode draw(const LinechartConfig& config, Layer* frame) {
  // setup domains
  DomainConfig domain_x;
  DomainConfig domain_y;

  // render axes
  AxisDefinition axis_top;
  if (auto rc = axis_expand_auto(config.axis_top, domain_x, &axis_top); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_top, AxisPosition::TOP, frame); rc) {
    return rc;
  }

  AxisDefinition axis_right;
  if (auto rc = axis_expand_auto(config.axis_right, domain_y, &axis_right); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_right, AxisPosition::RIGHT, frame); rc) {
    return rc;
  }

  AxisDefinition axis_bottom;
  if (auto rc = axis_expand_auto(config.axis_bottom, domain_x, &axis_bottom); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_bottom, AxisPosition::BOTTOM, frame); rc) {
    return rc;
  }

  AxisDefinition axis_left;
  if (auto rc = axis_expand_auto(config.axis_left, domain_y, &axis_left); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_left, AxisPosition::LEFT, frame); rc) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode configureAxisTop(const plist::Property& prop, LinechartConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_top.mode);
}

ReturnCode configureAxisRight(const plist::Property& prop, LinechartConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_right.mode);
}

ReturnCode configureAxisBottom(const plist::Property& prop, LinechartConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_bottom.mode);
}

ReturnCode configureAxisLeft(const plist::Property& prop, LinechartConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_left.mode);
}

ReturnCode configure(const plist::PropertyList& plist, ElementRef* elem) {
  static const PropertyDefinitions<LinechartConfig> pdefs = {
    {"axis-top", &configureAxisTop},
    {"axis-right", &configureAxisRight},
    {"axis-bottom", &configureAxisBottom},
    {"axis-left", &configureAxisLeft},
  };

  LinechartConfig config;
  if (auto rc = configureProperties(plist, pdefs, &config); !rc.isSuccess()) {
    return rc;
  }

  auto e = std::make_unique<Element>();
  e->draw = std::bind(&draw, config, std::placeholders::_1);
  *elem = std::move(e);

  return ReturnCode::success();
}

} // namespace linechart
} // namespace plotfx

