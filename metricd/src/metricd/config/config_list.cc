/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/config/config_list.h"

namespace fnordmetric {

std::map<MetricIDType, MetricConfig>& ConfigList::getMetricConfigs() {
  return metric_configs_;
}

const std::map<MetricIDType, MetricConfig>& ConfigList::getMetricConfigs()
    const {
  return metric_configs_;
}

const MetricConfig* ConfigList::getMetricConfig(MetricIDType metric_id) const {
  auto metric_config = metric_configs_.find(metric_id);
  if (metric_config == metric_configs_.end()) {
    return nullptr;
  } else {
    return &metric_config->second;
  }
}

void ConfigList::addMetricConfig(MetricIDType metric_id, MetricConfig config) {
  metric_configs_.emplace(metric_id, std::move(config));
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

const std::map<std::string, std::unique_ptr<SensorConfig>>& ConfigList::getSensorConfigs()
    const {
  return sensor_configs_;
}

const SensorConfig* ConfigList::getSensorConfig(std::string sensor_id) const {
  auto sensor_config = sensor_configs_.find(sensor_id);
  if (sensor_config == sensor_configs_.end()) {
    return nullptr;
  } else {
    return sensor_config->second.get();
  }
}

void ConfigList::addSensorConfig(std::unique_ptr<SensorConfig> config) {
  auto sensor_id = config->sensor_id;
  sensor_configs_.emplace(sensor_id, std::move(config));
}

} // namespace fnordmetric

