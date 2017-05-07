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

struct PlotSeries {
  std::string series_name;
  Timeseries<double> data;
};

struct Plot {
  uint64_t time_begin;
  uint64_t time_limit;

  std::vector<PlotSeries> series;
};

class PlotBuilder {
public:

  PlotBuilder(
      ConfigList* config,
      Backend* backend);

  ReturnCode addArgument(const std::string& key, const std::string& value);

  ReturnCode getPlot(Plot* plot);

protected:

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

} // namespace fnordmetric

