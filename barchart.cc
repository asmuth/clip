/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ui/barchart.h>

namespace fnordmetric {
namespace ui {

BarChart::BarChart(
    ui::Canvas* canvas) :
    Drawable(canvas),
    orientation_(O_VERTICAL),
    stacked_(false) {}

void BarChart::setOrientation(kBarChartOrientation orientation) {
  orientation_ = orientation;
}

void BarChart::setStacked(bool stacked) {
  stacked_ = stacked;
}

}
}
