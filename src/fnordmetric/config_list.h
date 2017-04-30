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
#include "fnordmetric/util/return_code.h"
#include "fnordmetric/metric.h"
#include "fnordmetric/units.h"
#include "fnordmetric/ingest.h"

namespace fnordmetric {

class ConfigList {
public:

  ConfigList();

  const std::string& getBackendURL() const;
  void setBackendURL(const std::string& backend_url);

  std::map<MetricIDType, MetricConfig>& getMetricConfigs();
  const std::map<MetricIDType, MetricConfig>& getMetricConfigs() const;
  const MetricConfig* getMetricConfig(MetricIDType metric_id) const;
  void addMetricConfig(MetricConfig config);

  const std::map<std::string, UnitConfig>& getUnitConfigs() const;
  const UnitConfig* getUnitConfig(std::string unit_id) const;
  void addUnitConfig(UnitConfig config);

  const std::vector<std::unique_ptr<IngestionTaskConfig>>& getIngestionTaskConfigs() const;
  void addIngestionTaskConfig(std::unique_ptr<IngestionTaskConfig> config);

protected:
  std::string backend_url_;
  bool create_metrics_;
  std::map<MetricIDType, MetricConfig> metric_configs_;
  std::map<std::string, UnitConfig> unit_configs_;
  std::vector<std::unique_ptr<IngestionTaskConfig>> ingestion_configs_;
};

} // namespace fnordmetric

