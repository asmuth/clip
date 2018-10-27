/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include "canvas.h"
#include "linechart.h"
#include <stx/exception.h>

namespace stx {
namespace chart {

char LineChart::kDefaultLineStyle[] = "solid";
char LineChart::kDefaultLineWidth[] = "2";
char LineChart::kDefaultPointStyle[] = "none";
char LineChart::kDefaultPointSize[] = "3";

LineChart::LineChart(
    chart::Canvas* canvas) :
    Drawable(canvas),
    show_labels_(false) {}

void LineChart::setLabels(bool show_labels) {
  show_labels_ = show_labels;
}

}
}
