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
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnord/chart/barchart.h>

namespace fnordmetric {
namespace query {

BarChartBuilder::BarChartBuilder(
    fnord::chart::Canvas* canvas,
    DrawStatement const* draw_stmt) :
    ChartBuilder(canvas, draw_stmt) {}

fnord::chart::Drawable* BarChartBuilder::getChart() const {
  auto chart = dynamic_cast<fnord::chart::BarChart*>(findChartType());
  setOrientation(chart);
  setStacked(chart);
  setLabels(chart);
  return chart;
}

fnord::chart::Drawable* BarChartBuilder::findChartType() const {
  preconditionCheck();

  if (auto c = tryType2D<fnord::chart::BarChart2D<
        fnordmetric::StringType,
        fnordmetric::FloatType>>())
    return c;

  if (auto c = tryType3D<fnord::chart::BarChart3D<
        fnordmetric::StringType,
        fnordmetric::FloatType,
        fnordmetric::FloatType>>())
    return c;

  invalidType();
  return nullptr;
}

void BarChartBuilder::setOrientation(fnord::chart::BarChart* chart) const {
  auto prop = draw_stmt_->getProperty(Token::T_ORIENTATION);

  if (prop == nullptr) {
    return;
  }

  switch (prop->getToken()->getType()) {
    case Token::T_VERTICAL:
      chart->setOrientation(fnord::chart::BarChart::O_VERTICAL);
      break;

    case Token::T_HORIZONTAL:
      chart->setOrientation(fnord::chart::BarChart::O_HORIZONTAL);
      break;

    default:
      break;
  }
}

void BarChartBuilder::setStacked(fnord::chart::BarChart* chart) const {
  auto prop = draw_stmt_->getProperty(Token::T_STACKED);
  chart->setStacked(prop != nullptr);
}

std::string BarChartBuilder::chartName() const {
  return "BarChart";
}

void BarChartBuilder::setLabels(fnord::chart::BarChart* chart) const {
  auto prop = draw_stmt_->getProperty(Token::T_LABELS);
  chart->setLabels(prop != nullptr);
}

}
}
