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

namespace fnordmetric {

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

} // namespace fnordmetric

