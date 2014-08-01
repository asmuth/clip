/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "canvas.h"
#include "pointchart.h"
#include "rendertarget.h"

namespace fnordmetric {
namespace ui {

double PointChart::kDefaultPointSize = 3.0f;
char PointChart::kDefaultPointStyle[] = "circle";

PointChart::PointChart(
    Canvas* canvas,
    NumericalDomain* x_domain /* = nullptr */,
    NumericalDomain* y_domain /* = nullptr */) :
    canvas_(canvas),
    x_domain_(x_domain),
    y_domain_(y_domain),
    num_series_(0) {}

void PointChart::addSeries(
      Series2D<double, double>* series,
      const std::string& point_style /* = kDefaultPointStyle */,
      double point_size /* = kDefaultPointSize */) {
  const auto& color = seriesColor(series);

  for (const auto& spoint : series->getData()) {
    Point point;
    point.x = std::get<0>(spoint);
    point.y = std::get<1>(spoint);
    point.size = point_size;
    point.type = point_style; // FIXPAUL too many copies
    point.color = color; // FIXPAUL too many copies
    points_.emplace_back(point);
  }
}

void PointChart::addSeries(
    Series3D<double, double, double>* series,
    const std::string& point_style /* = kDefaultPointStyle */) {
  std::string color = seriesColor(series);

  for (const auto& spoint : series->getData()) {
    Point point;
    point.x = std::get<0>(spoint);
    point.y = std::get<1>(spoint);
    point.size = std::get<2>(spoint);
    point.type = point_style; // FIXPAUL too many copies
    point.color = color; // FIXPAUL too many copies
    points_.emplace_back(point);
  }
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

void PointChart::render(
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

  target->beginGroup("points");

  for (const auto& point : points_) {
    auto draw_x = padding_left + x_domain->scale(point.x) * inner_width;
    auto draw_y = padding_top + (1.0 - y_domain->scale(point.y)) * inner_height;

    target->drawPoint(
      draw_x,
      draw_y,
      point.type,
      point.size,
      point.color,
      "point");
  }

  target->finishGroup();
}

}
}
