/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "canvas.h"
#include "areachart.h"
#include "rendertarget.h"

namespace fnordmetric {
namespace ui {

char AreaChart::kDefaultBorderStyle[] = "none";
double AreaChart::kDefaultBorderWidth = 2.0f;
char AreaChart::kDefaultPointStyle[] = "none";
double AreaChart::kDefaultPointSize = 3.0f;

AreaChart::AreaChart(
    Canvas* canvas,
    NumericalDomain* x_domain /* = nullptr */,
    NumericalDomain* y_domain /* = nullptr */,
    bool stacked /* = false */) :
    canvas_(canvas),
    x_domain_(x_domain),
    y_domain_(x_domain),
    stacked_(stacked),
    num_series_(0) {}

void AreaChart::addSeries(
      Series2D<double, double>* series,
      const std::string& border_style /* = kDefaultBorderStyle */,
      double border_width /* = kDefaultBorderWidth */,
      const std::string& point_style /* = kDefaultPointStyle */,
      double point_size /* = kDefaultPointSize */,
      bool smooth /* = false */) {
  Area area;
  area.color = seriesColor(series);
  area.border_style = border_style;
  area.border_width = border_width;
  area.point_style = point_style;
  area.point_size = point_size;
  area.smooth = smooth;

  // FIXPAUL: stacked
  for (const auto& spoint : series->getData()) {
    area.points.emplace_back(std::get<0>(spoint), 0, std::get<1>(spoint));
  }

  areas_.emplace_back(area);
}

AxisDefinition* AreaChart::addAxis(AxisDefinition::kPosition position) {
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

NumericalDomain* AreaChart::getXDomain() const {
  if (x_domain_ != nullptr) {
    return x_domain_;
  }

  if (x_domain_auto_.get() == nullptr) {
    double x_min = 0.0f;
    double x_max = 0.0f;

    for (const auto& area : areas_) {
      for (const auto& point : area.points) {
        if (std::get<0>(point) > x_max) {
          x_max = std::get<0>(point);
        }
        if (std::get<0>(point) < x_min) {
          x_min = std::get<0>(point) ;
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

NumericalDomain* AreaChart::getYDomain() const {
  if (y_domain_ != nullptr) {
    return y_domain_;
  }

  if (y_domain_auto_.get() == nullptr) {
    double y_min = 0.0f;
    double y_max = 0.0f;

    for (const auto& area : areas_) {
      for (const auto& point : area.points) {
        if (std::get<2>(point) > y_max) {
          y_max = std::get<2>(point);
        }
        if (std::get<2>(point) < y_min) {
          y_min = std::get<2>(point) ;
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

void AreaChart::render(
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

  target->beginGroup("areas");

  for (const auto& area : areas_) {
    std::vector<std::pair<double, double>> area_coords;
    std::vector<std::pair<double, double>> border_top_coords;
    std::vector<std::pair<double, double>> point_coords;

    for (int i = 0; i < area.points.size(); ++i) {
      const auto& point = area.points[i];
      auto scaled_x = x_domain->scale(std::get<0>(point));
      auto scaled_y2 = 1.0 - y_domain->scale(std::get<2>(point));
      auto draw_x = padding_left + scaled_x * inner_width;
      auto draw_y2 = padding_top + scaled_y2 * inner_height;

      area_coords.emplace_back(draw_x, draw_y2);
      border_top_coords.emplace_back(draw_x, draw_y2);
      point_coords.emplace_back(draw_x, draw_y2);
    }

    for (int i = area.points.size() - 1; i >= 0; --i) {
      const auto& point = area.points[i];
      auto scaled_x = x_domain->scale(std::get<0>(point));
      auto scaled_y1 = 1.0 - y_domain->scale(std::get<1>(point));
      auto draw_x = padding_left + scaled_x * inner_width;
      auto draw_y1 = padding_top + scaled_y1 * inner_height;

      area_coords.emplace_back(draw_x, draw_y1);

      if (std::get<1>(point) != 0) {
        border_top_coords.emplace_back(draw_x, draw_y1);
        point_coords.emplace_back(draw_x, draw_y1);
      }
    }

    target->drawPath(
      area_coords,
      "fill",
      area.border_style == "none" ? 0 : area.border_width,
      area.smooth,
      area.color,
      "area");

    if (area.border_style != "none") {
      target->drawPath(
        border_top_coords,
        area.border_style,
        area.border_width,
        area.smooth,
        area.color,
        "line");
    }

    if (area.point_style != "none") {
      for (const auto &point : point_coords) {
        target->drawPoint(
          point.first,
          point.second,
          area.point_style,
          area.point_size,
          area.color,
          "point");
      }
    }
  }

  target->finishGroup();
}

}
}
