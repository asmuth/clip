/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/sql_extensions/linechartbuilder.h>
#include <metricd/sql_extensions/drawstatement.h>
#include <metricd/ui/linechart.h>

namespace fnordmetric {
namespace query {

LineChartBuilder::LineChartBuilder(
    ui::Canvas* canvas,
    DrawStatement const* draw_stmt) :
    ChartBuilder(canvas, draw_stmt) {}

ui::Drawable* LineChartBuilder::getChart() const {
  auto chart = dynamic_cast<ui::LineChart*>(findChartType());
  setLabels(chart);
  return chart;
}

ui::Drawable* LineChartBuilder::findChartType() const {
  preconditionCheck();

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::TimeType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::TimeType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::TimeType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::FloatType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::FloatType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::FloatType,
        fnordmetric::StringType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::StringType,
        fnordmetric::TimeType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType2D<ui::LineChart2D<
        fnordmetric::StringType,
        fnordmetric::StringType>>())
    return c;

  invalidType();
  return nullptr;
}

std::string LineChartBuilder::chartName() const {
  return "LineChart";
}

void LineChartBuilder::setLabels(ui::LineChart* chart) const {
  auto prop = draw_stmt_->getProperty(Token::T_LABELS);
  chart->setLabels(prop != nullptr);
}

}
}
