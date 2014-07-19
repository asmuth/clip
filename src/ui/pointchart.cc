/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "canvas.h"
#include "pointchart.h"

namespace fnordmetric {
namespace ui {

PointChart::PointChart(
    Canvas* canvas,
    NumericalDomain* x_domain /* = nullptr */,
    NumericalDomain* y_domain /* = nullptr */) :
    canvas_(canvas),
    x_domain_(x_domain),
    y_domain_(x_domain),
    num_series_(0) {}

void PointChart::addSeries(Series2D<double, double>* series) {
  for (const auto& spoint : series->getData()) {
    Point point;
    point.x = std::get<0>(spoint);
    point.y = std::get<1>(spoint);
    points_.emplace_back(point);
  }
}

void PointChart::addSeries(Series3D<double, double, double>* series) {
}

AxisDefinition* PointChart::addAxis(AxisDefinition::kPosition position) {
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

NumericalDomain* PointChart::getXDomain() const {
  if (x_domain_ != nullptr) {
    return x_domain_;
  }

  if (x_domain_auto_.get() == nullptr) {
    double x_min = 0.0f;
    double x_max = 0.0f;

    for (const auto& point : points_) {
      if (point.x > x_max) {
        x_max = point.x;
      }
      if (point.x < x_min) {
        x_min = point.x;
      }
    }

    x_domain_auto_.reset(new NumericalDomain(x_min, x_max, false));
  }

  return x_domain_auto_.get();
}

NumericalDomain* PointChart::getYDomain() const {
  if (y_domain_ != nullptr) {
    return y_domain_;
  }

  if (y_domain_auto_.get() == nullptr) {
    double y_min = 0.0f;
    double y_max = 0.0f;

    for (const auto& point : points_) {
      if (point.y > y_max) {
        y_max = point.y;
      }
      if (point.y < y_min) {
        y_min = point.y;
      }
    }

    y_domain_auto_.reset(new NumericalDomain(y_min, y_max, false));
  }


  return y_domain_auto_.get();
}

void PointChart::render(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  for (const auto& point : points_) {
  }
}

}
}
