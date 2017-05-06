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
  Timeseries<double> data;
};

struct Plot {
  std::vector<PlotSeries> series;
};

class PlotBuilder {
public:

  PlotBuilder(
      ConfigList* config,
      Backend* backend);

  ReturnCode addArgument(const std::string& key, const std::string& value);

  ReturnCode addMetric(const std::string& metric_id);

  Plot&& getPlot();

protected:
  Plot plot_;
  ConfigList* config_;
  Backend* backend_;
};

ReturnCode renderPlot(const Plot* plot);

} // namespace fnordmetric

