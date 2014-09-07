/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/pointchartbuilder.h>
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/ui/pointchart.h>

namespace fnordmetric {
namespace query {

PointChartBuilder::PointChartBuilder(
    ui::Canvas* canvas,
    DrawStatement const* draw_stmt) :
    ChartBuilder(canvas, draw_stmt) {}

ui::Drawable* PointChartBuilder::getChart() const {
  preconditionCheck();

  if (auto c = tryType2D<ui::PointChart2D<double, double>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<double, std::string>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<std::string, double>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<std::string, std::string>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<double, double, double>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<double, std::string, double>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<std::string, double, double>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<std::string, std::string, double>>())
    return c;

  invalidType();
  return nullptr;
}

std::string PointChartBuilder::chartName() const {
  return "PointChart";
}

}
}
