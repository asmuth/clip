/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/barchartbuilder.h>
#include <fnordmetric/ui/barchart.h>

namespace fnordmetric {
namespace query {

BarChartBuilder::BarChartBuilder(ui::Canvas* canvas) : ChartBuilder(canvas) {}

ui::Drawable* BarChartBuilder::getChart() const {
  if (auto c = tryType3D<ui::BarChart3D<std::string, int, int>>())
    return c;

  if (auto c = tryType3D<ui::BarChart3D<std::string, double, double>>())
    return c;

  invalidType();
}

}
}
