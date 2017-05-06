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
#include <metrictools/util/time.h>
#include <metrictools/timeseries.h>
#include "libcplot/libcplot.h"

namespace fnordmetric {

using namespace cplot;

PlotBuilder::PlotBuilder(
    ConfigList* config,
    Backend* backend) :
    config_(config),
    backend_(backend) {
  plot_.time_limit = WallClock::unixMicros();
  plot_.time_begin = plot_.time_limit - 2 * kMicrosPerHour;
}

ReturnCode PlotBuilder::addArgument(
    const std::string& key,
    const std::string& value) {
  if (key == "metric") {
    return addMetric(value);
  }

  return ReturnCode::errorf("EARG", "invalid argument: $0", key);
}

ReturnCode PlotBuilder::addMetric(const std::string& metric_id) {
  s_metric_ = metric_id;
  return ReturnCode::success();
}

ReturnCode PlotBuilder::flush() {
  if (s_metric_.empty()) {
    return ReturnCode::error("EARG", "no metric");
  }

  auto metric = config_->getMetricConfig(s_metric_);
  if (!metric) {
    return ReturnCode::errorf(
        "EARG",
        "metric not found: '$0'",
        s_metric_);
  }

  FetchStorageOp op(config_->getGlobalConfig());
  {
    FetchStorageOp::FetchRequest req;
    req.metric = metric;
    req.fetch_history = true;
    req.history_time_begin = plot_.time_begin;
    req.history_time_limit = plot_.time_limit;

    op.addRequest(std::move(req));

    auto rc = backend_->performOperation(&op);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  for (const auto& res : op.getResponses()) {
    PlotSeries series;
    series.series_name = "series";
    auto rc = convertTimeseries(res.history, &series.data);
    if (!rc.isSuccess()) {
      return rc;
    }

    plot_.series.emplace_back(series);
  }

  s_metric_.clear();
  return ReturnCode::success();
}

ReturnCode PlotBuilder::getPlot(Plot* plot) {
  auto rc = flush();
  if (!rc.isSuccess()) {
    return rc;
  }

  *plot = std::move(plot_);
  return ReturnCode::success();
}

ReturnCode renderPlot(const Plot* plot, std::string* out) {
  Canvas chart;
  TimeDomain x_domain(plot->time_begin, plot->time_limit);
  ContinuousDomain<double> y_domain(0, 10.0, false);

  auto line_chart = chart.addChart<LineChart2D<UnixTime, double>>(
      &x_domain, &y_domain);

  for (const auto& series : plot->series) {
    auto s = new Series2D<UnixTime, double>(series.series_name);
    for (size_t i = 0; i < series.data.size(); ++i) {
      s->addDatum(series.data.timestamps[i], series.data.values[i]);
    }

    line_chart->addSeries(s);
  }

  line_chart->addAxis(AxisDefinition::TOP);
  line_chart->addAxis(AxisDefinition::RIGHT);
  line_chart->addAxis(AxisDefinition::BOTTOM);
  line_chart->addAxis(AxisDefinition::LEFT);

  SVGTarget target(out);
  chart.render(&target);

  return ReturnCode::success();
}

} // namespace fnordmetric

