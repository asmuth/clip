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
#include "areachart.h"
#include "rendertarget.h"

namespace stx {
namespace chart {

char AreaChart::kDefaultLineStyle[] = "none";
char AreaChart::kDefaultLineWidth[] = "1";
char AreaChart::kDefaultPointStyle[] = "none";
char AreaChart::kDefaultPointSize[] = "2";

AreaChart::AreaChart(
    chart::Canvas* canvas,
    bool stacked) :
    Drawable(canvas),
    stacked_(stacked) {}

}
}
