
/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "canvas.h"
#include "linechart.h"
#include "rendertarget.h"

namespace fnordmetric {
namespace ui {

char LineChart::kDefaultLineStyle[] = "solid";
double LineChart::kDefaultLineWidth = 2.0f;
char LineChart::kDefaultPointStyle[] = "none";
double LineChart::kDefaultPointSize = 3.0f;

LineChart::LineChart(
    Canvas* canvas,
    NumericalDomain* x_domain /* = nullptr */,
    NumericalDomain* y_domain /* = nullptr */) :
    canvas_(canvas),
    x_domain_(x_domain),
    y_domain_(y_domain),
    num_series_(0) {}

void LineChart::addSeries(
      Series2D<double, double>* series,
      const std::string& line_style /* = kDefaultLineStyle */,
      double line_width /* = kDefaultLineWidth */,
      const std::string& point_style /* = kDefaultLineStyle */,
      double point_size /* = kDefaultPointsize */,
      bool smooth /* = false */) {
  Line line;
  line.color = seriesColor(series);
  line.line_style = line_style;
  line.line_width = line_width;
  line.point_style = point_style;
  line.point_size = point_size;
  line.smooth = smooth;

  for (const auto& spoint : series->getData()) {
    line.points.emplace_back(std::get<0>(spoint), std::get<1>(spoint));
  }

  lines_.emplace_back(line);
}

AxisDefinition* LineChart::addAxis(AxisDefinition::kPosition position) {
  switch (position) {
    case AxisDefinition::TOP:
      return canvas_->addAxis(position, getXDomain());

    case AxisDefinition::RIGHT:
      return canvas_->addAxis(position, getYDomain());

    case AxisDefinition::BOTTOM:
      return canvas_->addAxis(position, getXDomain());

    case AxisDefinition::LEFT:
      return canvas_->addAxis(position, getYDomain());
  }
}

NumericalDomain* LineChart::getXDomain() const {
  if (x_domain_ != nullptr) {
    return x_domain_;
  }

  if (x_domain_auto_.get() == nullptr) {
    double x_min = 0.0f;
    double x_max = 0.0f;

    for (const auto& line : lines_) {
      for (const auto& point : line.points) {
        if (point.first > x_max) {
          x_max = point.first;
        }
        if (point.first < x_min) {
          x_min = point.first;
        }
      }
    }

    if (x_max > 0) {
      x_max *= 1.1;
    }

    if (x_max < 0) {
      x_max *= 0.9;
    }

    if (x_min > 0) {
      x_min *= 0.9;
    }

    if (x_min < 0) {
      x_min *= 1.1;
    }

    x_domain_auto_.reset(new NumericalDomain(x_min, x_max, false));
  }

  return x_domain_auto_.get();
}

NumericalDomain* LineChart::getYDomain() const {
  if (y_domain_ != nullptr) {
    return y_domain_;
  }

  if (y_domain_auto_.get() == nullptr) {
    double y_min = 0.0f;
    double y_max = 0.0f;

    for (const auto& line : lines_) {
      for (const auto& point : line.points) {
        if (point.second > y_max) {
          y_max = point.second;
        }
        if (point.second < y_min) {
          y_min = point.second;
        }
      }
    }

    if (y_max > 0) {
      y_max *= 1.1;
    }

    if (y_max < 0) {
      y_max *= 0.9;
    }

    if (y_min > 0) {
      y_min *= 0.9;
    }

    if (y_min < 0) {
      y_min *= 1.1;
    }

    y_domain_auto_.reset(new NumericalDomain(y_min, y_max, false));
  }


  return y_domain_auto_.get();
}

void LineChart::render(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  auto padding_top = std::get<0>(*padding);
  auto padding_right = std::get<1>(*padding);
  auto padding_bottom = std::get<2>(*padding);
  auto padding_left = std::get<3>(*padding);
  auto inner_width = width - padding_right - padding_left;
  auto inner_height = height - padding_top - padding_bottom;
  auto x_domain = getXDomain();
  auto y_domain = getYDomain();

  target->beginGroup("lines");

  for (const auto& line : lines_) {
    std::vector<std::pair<double, double>> coords;

    for (const auto& point : line.points) {
      coords.emplace_back(
          padding_left + x_domain->scale(point.first) * inner_width,
          padding_top + (1.0 - y_domain->scale(point.second)) * inner_height);
    }

    target->drawPath(
      coords,
      line.line_style,
      line.line_width,
      line.smooth,
      line.color,
      "line");

    if (line.point_style != "none") {
      for (const auto& point : coords) {
        target->drawPoint(
          point.first,
          point.second,
          line.point_style,
          line.point_size,
          line.color,
          "point");
      }
    }
  }

  target->finishGroup();
}

}
}
