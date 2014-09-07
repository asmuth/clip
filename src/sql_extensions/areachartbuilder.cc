/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/areachartbuilder.h>
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/ui/areachart.h>

namespace fnordmetric {
namespace query {

AreaChartBuilder::AreaChartBuilder(
    ui::Canvas* canvas,
    DrawStatement const* draw_stmt) :
    ChartBuilder(canvas, draw_stmt) {}

ui::Drawable* AreaChartBuilder::getChart() const {
  preconditionCheck();

  if (auto c = tryType2D<ui::AreaChart2D<double, double>>())
    return c;

  if (auto c = tryType2D<ui::AreaChart2D<std::string, double>>())
    return c;

  if (auto c = tryType3D<ui::AreaChart3D<double, double, double>>())
    return c;

  if (auto c = tryType3D<ui::AreaChart3D<std::string, double, double>>())
    return c;

  invalidType();
  return nullptr;
}

std::string AreaChartBuilder::chartName() const {
  return "AreaChart";
}

}
}
