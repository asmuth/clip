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
#include <fnord/chart/areachart.h>

namespace fnordmetric {
namespace query {

AreaChartBuilder::AreaChartBuilder(
    fnord::chart::Canvas* canvas,
    DrawStatement const* draw_stmt) :
    ChartBuilder(canvas, draw_stmt) {}

fnord::chart::Drawable* AreaChartBuilder::getChart() const {
  preconditionCheck();

  if (auto c = tryType2D<fnord::chart::AreaChart2D<
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<fnord::chart::AreaChart2D<
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<fnord::chart::AreaChart2D<
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<fnord::chart::AreaChart3D<
        fnordmetric::TimeType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<fnord::chart::AreaChart3D<
        fnordmetric::FloatType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<fnord::chart::AreaChart3D<
        fnordmetric::StringType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  invalidType();
  return nullptr;
}

std::string AreaChartBuilder::chartName() const {
  return "AreaChart";
}

}
}
