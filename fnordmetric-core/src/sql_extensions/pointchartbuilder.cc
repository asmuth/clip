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
  auto chart = dynamic_cast<ui::PointChart*>(findChartType());
  setLabels(chart);
  return chart;
}

ui::Drawable* PointChartBuilder::findChartType() const {
  preconditionCheck();

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::TimeType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::TimeType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::FloatType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::FloatType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::StringType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::PointChart2D<
        fnordmetric::StringType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::TimeType,
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::TimeType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::TimeType,
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::FloatType,
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::FloatType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::FloatType,
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::StringType,
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::StringType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<ui::PointChart3D<
        fnordmetric::StringType,
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  invalidType();
  return nullptr;
}

std::string PointChartBuilder::chartName() const {
  return "PointChart";
}

void PointChartBuilder::setLabels(ui::PointChart* chart) const {
  auto prop = draw_stmt_->getProperty(Token::T_LABELS);
  chart->setLabels(prop != nullptr);
}

}
}
