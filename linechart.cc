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
#include "linechart.h"
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace ui {

char LineChart::kDefaultLineStyle[] = "solid";
double LineChart::kDefaultLineWidth = 2.0f;
char LineChart::kDefaultPointStyle[] = "none";
double LineChart::kDefaultPointSize = 3.0f;

LineChart::LineChart(ui::Canvas* canvas) : Drawable(canvas) {}

}
}
