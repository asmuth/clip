/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <vector>
#include <metrictools/timeseries.h>
#include <metrictools/config_list.h>
#include <metrictools/storage/backend.h>

namespace fnordmetric {

enum class PlotOutputFormat { SVG, IFRAME };

enum class PlotChartType { LINE, AREA };

struct PlotSeries {
  std::string series_name;
  Timeseries<double> data;
};

struct PlotSeriesGroup {
  PlotChartType chart_type;
  std::vector<PlotSeries> series;
};

struct Plot {
  uint32_t width;
  uint32_t height;
  PlotOutputFormat output_format;

  uint64_t time_begin;
  uint64_t time_limit;

  std::vector<PlotSeriesGroup> series_groups;
};

class PlotBuilder {
public:

  PlotBuilder(
      ConfigList* config,
      Backend* backend);

  ReturnCode addArgument(const std::string& key, const std::string& value);

  ReturnCode getPlot(Plot* plot);

protected:

  ReturnCode setFormat(const std::string& p);
  ReturnCode setWidth(const std::string& p);
  ReturnCode setHeight(const std::string& p);

  ReturnCode setFrom(const std::string& p);
  ReturnCode setUntil(const std::string& p);

  ReturnCode addMetric(const std::string& metric_id);
  ReturnCode flush();

  Plot plot_;
  ConfigList* config_;
  Backend* backend_;

  std::string s_metric_;
};

ReturnCode renderPlot(const Plot* plot, std::string* out);

bool parsePlotOutputFormat(const std::string& str, PlotOutputFormat* fmt);

} // namespace fnordmetric

