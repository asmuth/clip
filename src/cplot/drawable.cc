/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include "cplot/canvas.h"
#include "cplot/drawable.h"

namespace stx {
namespace chart {

Drawable::Drawable(Canvas* canvas) : canvas_(canvas) {}

Drawable::~Drawable() {
  for (auto series : all_series_) {
    delete series;
  }
}

void Drawable::setTitle(const std::string& title) {
  canvas_->setTitle(title);
}

void Drawable::setSubtitle(const std::string& subtitle) {
  canvas_->setSubtitle(subtitle);
}

LegendDefinition* Drawable::addLegend(
    LegendDefinition::kVerticalPosition vert_pos,
    LegendDefinition::kHorizontalPosition horiz_pos,
    LegendDefinition::kPlacement placement,
    const std::string& title) {
  auto legend = canvas_->addLegend(vert_pos, horiz_pos, placement, title);
  updateLegend();
  return legend;
}

void Drawable::addSeries(Series* series) {
  all_series_.push_back(series);
  updateLegend();
}

void Drawable::updateLegend() {
  auto legend = canvas_->legend();

  if (legend == nullptr) {
    return;
  }

  for (const auto& series : all_series_) {
    legend->addEntry(
        series->name(),
        series->getProperty(Series::P_COLOR),
        "circle");
  }
}

}
}
