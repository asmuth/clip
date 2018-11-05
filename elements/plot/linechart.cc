/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include "linechart.h"

namespace signaltk {
namespace chart {

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

}
}
