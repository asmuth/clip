/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metrictools/config_list.h"

namespace fnordmetric {

ConfigList::ConfigList() : global_config_(new GlobalConfig()) {}

const std::string& ConfigList::getBackendURL() const {
  return backend_url_;
}

void ConfigList::setBackendURL(const std::string& backend_url) {
  backend_url_ = backend_url;
}


const std::map<MetricIDType, std::shared_ptr<MetricConfig>>& ConfigList::getMetricConfigs()
    const {
  return metric_configs_;
}

std::shared_ptr<const MetricConfig> ConfigList::getMetricConfig(MetricIDType metric_id) const {
  auto metric_config = metric_configs_.find(metric_id);
  if (metric_config == metric_configs_.end()) {
    return nullptr;
  } else {
    return metric_config->second;
  }
}

void ConfigList::addMetricConfig(MetricConfig config) {
  auto metric_id = config.metric_id;

  metric_configs_.emplace(
      metric_id,
      std::make_shared<MetricConfig>(std::move(config)));
}

const std::map<std::string, UnitConfig>& ConfigList::getUnitConfigs()
    const {
  return unit_configs_;
}

const UnitConfig* ConfigList::getUnitConfig(std::string unit_id) const {
  auto unit_config = unit_configs_.find(unit_id);
  if (unit_config == unit_configs_.end()) {
    return nullptr;
  } else {
    return &unit_config->second;
  }
}

void ConfigList::addUnitConfig(UnitConfig config) {
  auto unit_id = config.unit_id;
  unit_configs_.emplace(unit_id, std::move(config));
}

const std::vector<std::unique_ptr<IngestionTaskConfig>>& ConfigList::getIngestionTaskConfigs() const {
  return ingestion_configs_;
}

void ConfigList::addIngestionTaskConfig(std::unique_ptr<IngestionTaskConfig> config) {
  ingestion_configs_.emplace_back(std::move(config));
}

const std::vector<std::unique_ptr<ListenerConfig>>& ConfigList::getListenerConfigs() const {
  return listener_configs_;
}

void ConfigList::addListenerConfig(std::unique_ptr<ListenerConfig> config) {
  listener_configs_.emplace_back(std::move(config));
}

void ConfigList::addDashboardPath(const std::string& path) {
  dashboard_paths_.insert(path);
}

const std::set<std::string>& ConfigList::getDashboardPaths() const {
  return dashboard_paths_;
}

std::shared_ptr<const GlobalConfig> ConfigList::getGlobalConfig() const noexcept {
  return global_config_;
}

std::shared_ptr<GlobalConfig> ConfigList::getGlobalConfig() noexcept {
  return global_config_;
}

} // namespace fnordmetric

