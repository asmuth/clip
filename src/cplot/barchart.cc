/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "cplot/barchart.h"

namespace stx {
namespace chart {

BarChart::BarChart(
    chart::Canvas* canvas,
    kBarChartOrientation orient /* = O_VERTICAL */,
    bool stacked /* = false */) :
    Drawable(canvas),
    orientation_(orient),
    stacked_(stacked),
    show_labels_(false) {}

void BarChart::setOrientation(kBarChartOrientation orientation) {
  orientation_ = orientation;
}

void BarChart::setLabels(bool show_labels) {
  show_labels_ = show_labels;
}

}
}
