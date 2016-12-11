/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <map>
#include <memory>
#include <string>
#include "metricd/util/return_code.h"
#include "metricd/metric.h"

namespace fnordmetric {

class ConfigList {
public:

  const std::map<MetricIDType, MetricConfig>& getMetricConfigs() const;
  void addMetricConfig(MetricIDType metric_id, MetricConfig config);

protected:
  std::map<MetricIDType, MetricConfig> metric_configs_;
};

} // namespace fnordmetric

