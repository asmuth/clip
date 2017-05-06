/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/plot.h>
#include "libcplot/libcplot.h"

namespace fnordmetric {

using namespace cplot;

PlotBuilder::PlotBuilder(
    ConfigList* config,
    Backend* backend) :
    config_(config),
    backend_(backend) {}

ReturnCode PlotBuilder::addArgument(
    const std::string& key,
    const std::string& value) {
  return ReturnCode::success();
}

ReturnCode PlotBuilder::addMetric(const std::string& metric_id) {
  return ReturnCode::success();
}

Plot&& PlotBuilder::getPlot() {
  return std::move(plot_);
}

ReturnCode renderPlot(const Plot* plot) {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas chart;
  auto line_chart = chart.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);
  line_chart->addSeries(series1);
  line_chart->addSeries(series2);
  line_chart->addAxis(AxisDefinition::TOP);
  line_chart->addAxis(AxisDefinition::RIGHT);
  line_chart->addAxis(AxisDefinition::BOTTOM);
  line_chart->addAxis(AxisDefinition::LEFT);

  std::string svg;
  SVGTarget target(&svg);
  chart.render(&target);

  std::cout << svg;

  return ReturnCode::success();
}

} // namespace fnordmetric

