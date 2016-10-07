/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQLEXTENSIONS_BARCHARTBUILDER_H
#define _FNORDMETRIC_SQLEXTENSIONS_BARCHARTBUILDER_H
#include <metricd/sql_extensions/chartbuilder.h>

namespace fnordmetric {
namespace query {
class DrawStatement;

class BarChartBuilder : public ChartBuilder {
public:
  BarChartBuilder(ui::Canvas* canvas, DrawStatement const* draw_stmt);
  ui::Drawable* getChart() const override;
  std::string chartName() const override;
protected:
  ui::Drawable* findChartType() const;
  void setOrientation(ui::BarChart* chart) const;
  void setStacked(ui::BarChart* chart) const;
  void setLabels(ui::BarChart* chart) const;
};

}
}
#endif
