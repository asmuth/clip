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
#include <metrictools/util/format.h>
#include "libcplot/libcplot.h"

namespace fnordmetric {

using namespace cplot;

PlotBuilder::PlotBuilder(
    ConfigList* config,
    Backend* backend) :
    config_(config),
    backend_(backend) {
  plot_.width = 800;
  plot_.height = 200;
  plot_.output_format = PlotOutputFormat::SVG;
  plot_.time_limit = WallClock::unixMicros();
  plot_.time_begin = plot_.time_limit - 2 * kMicrosPerHour;
  plot_.series_groups.emplace_back(PlotSeriesGroup{
    .chart_type = PlotChartType::LINE
  });
}

ReturnCode PlotBuilder::addArgument(
    const std::string& key,
    const std::string& value) {
  if (key == "format") {
    return setFormat(value);
  }

  if (key == "width") {
    return setWidth(value);
  }

  if (key == "height") {
    return setHeight(value);
  }

  if (key == "from") {
    return setFrom(value);
  }

  if (key == "until") {
    return setUntil(value);
  }

  if (key == "metric") {
    return addMetric(value);
  }

  return ReturnCode::errorf("EARG", "invalid argument: $0", key);
}

ReturnCode PlotBuilder::setFormat(const std::string& p) {
  if (parsePlotOutputFormat(p, &plot_.output_format)) {
    return ReturnCode::success();
  } else {
    return ReturnCode::error("EARG", "invalid argument for 'format'");
  }
}

ReturnCode PlotBuilder::setWidth(const std::string& p) try {
  plot_.width = std::stoul(p);
  return ReturnCode::success();
} catch (...) {
  return ReturnCode::error("EARG", "invalid argument for 'width'");
}

ReturnCode PlotBuilder::setHeight(const std::string& p) try {
  plot_.height = std::stoul(p);
  return ReturnCode::success();
} catch (...) {
  return ReturnCode::error("EARG", "invalid argument for 'height'");
}

ReturnCode PlotBuilder::setFrom(const std::string& p) {
  return parsePointInTime(p, &plot_.time_begin);
}

ReturnCode PlotBuilder::setUntil(const std::string& p) {
  return parsePointInTime(p, &plot_.time_limit);
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
    auto rc = convertTimeseries(res.history.get(), &series.data);
    if (!rc.isSuccess()) {
      return rc;
    }

    plot_.series_groups.back().series.emplace_back(series);
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

static ReturnCode renderPlotSeries(
    const Plot* plot,
    const PlotSeriesGroup* series_group,
    Canvas* chart,
    DomainProvider* x_domain,
    DomainProvider* y_domain) {
  auto line_chart = chart->addChart<LineChart2D<UnixTime, double>>(
      x_domain->get(),
      y_domain->get());

  for (const auto& series : series_group->series) {
    auto s = new Series2D<UnixTime, double>(series.series_name);
    for (size_t i = 0; i < series.data.size(); ++i) {
      s->addDatum(series.data.timestamps[i], series.data.values[i]);
    }

    line_chart->addSeries(s);
  }

  return ReturnCode::success();
}

static ReturnCode renderPlot_SVG(const Plot* plot, std::string* out) {
  Canvas chart;
  chart.setDimensions(plot->width, plot->height);

  DomainProvider x_domain(new TimeDomain(plot->time_begin, plot->time_limit));
  DomainProvider y_domain(new ContinuousDomain<double>(0, 1.0, false));

  for (const auto& series_group : plot->series_groups) {
    auto rc = renderPlotSeries(plot, &series_group, &chart, &x_domain, &y_domain);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    auto axis = chart.addAxis(AxisDefinition::TOP);
    axis->setDomain(&x_domain);
  }

  {
    auto axis = chart.addAxis(AxisDefinition::BOTTOM);
    axis->setDomain(&x_domain);
  }

  {
    auto axis = chart.addAxis(AxisDefinition::LEFT);
    axis->setDomain(&y_domain);
  }

  {
    auto axis = chart.addAxis(AxisDefinition::RIGHT);
    axis->setDomain(&y_domain);
  }

  SVGTarget target(out);
  chart.render(&target);

  return ReturnCode::success();
}

ReturnCode renderPlot(const Plot* plot, std::string* out) {
  switch (plot->output_format) {
    case PlotOutputFormat::SVG:
      return renderPlot_SVG(plot, out);
    default:
      return ReturnCode::error("EARG", "invalid output format");
  }
}

bool parsePlotOutputFormat(const std::string& str, PlotOutputFormat* fmt) {
  static const std::map<std::string, PlotOutputFormat> fmt_map = {
    { "svg", PlotOutputFormat::SVG },
    { "iframe", PlotOutputFormat::IFRAME },
  };

  auto iter = fmt_map.find(str);
  if (iter != fmt_map.end()) {
    *fmt = iter->second;
    return true;
  } else {
    return false;
  }
}

} // namespace fnordmetric

