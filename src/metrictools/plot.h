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

namespace fnordmetric {

struct PlotSeries {
};

struct Plot {

};

class PlotBuilder {
public:

  PlotBuilder(
      ConfigList* config,
      Backend* backend);

  void addMetric(const std::string& metric_id);

  const Plot* getPlot() const;

protected:
  Plot plot_;
  ConfigList* config_;
  Backend* backend_;
};

} // namespace fnordmetric

