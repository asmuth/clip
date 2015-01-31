/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQLEXTENSIONS_POINTCHARTBUILDER_H
#define _FNORDMETRIC_SQLEXTENSIONS_POINTCHARTBUILDER_H
#include <chartsql/chartbuilder.h>
#include <fnord-chart/pointchart.h>

namespace fnordmetric {
namespace ui {
class PointChart;
}

namespace query {
class DrawStatement;

class PointChartBuilder : public ChartBuilder {
public:
  PointChartBuilder(
      fnord::chart::Canvas* canvas,
      DrawStatement const* draw_stmt);

  fnord::chart::Drawable* getChart() const override;
  std::string chartName() const override;
protected:
  fnord::chart::Drawable* findChartType() const;
  void setLabels(fnord::chart::PointChart* chart) const;
};

}
}
#endif
