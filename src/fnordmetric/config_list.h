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
#include "fnordmetric/table.h"
#include "fnordmetric/units.h"
#include "fnordmetric/sensors.h"

namespace fnordmetric {

class ConfigList {
public:

  ConfigList();

  size_t getSensorThreads() const;
  void setSensorThreads(size_t threads);

  std::map<TableIDType, TableConfig>& getTableConfigs();
  const std::map<TableIDType, TableConfig>& getTableConfigs() const;
  const TableConfig* getTableConfig(TableIDType metric_id) const;
  void addTableConfig(TableIDType metric_id, TableConfig config);

  const std::map<std::string, UnitConfig>& getUnitConfigs() const;
  const UnitConfig* getUnitConfig(std::string unit_id) const;
  void addUnitConfig(UnitConfig config);

  const std::map<std::string, std::unique_ptr<SensorConfig>>& getSensorConfigs() const;
  const SensorConfig* getSensorConfig(std::string unit_id) const;
  void addSensorConfig(std::unique_ptr<SensorConfig> config);

protected:
  size_t sensor_threads_;
  std::map<TableIDType, TableConfig> metric_configs_;
  std::map<std::string, UnitConfig> unit_configs_;
  std::map<std::string, std::unique_ptr<SensorConfig>> sensor_configs_;
};

} // namespace fnordmetric

