/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnordmetric/config_list.h"

namespace fnordmetric {

ConfigList::ConfigList() : create_tables_(false) {}

const std::string& ConfigList::getBackendURL() const {
  return backend_url_;
}

void ConfigList::setBackendURL(const std::string& backend_url) {
  backend_url_ = backend_url;
}

bool ConfigList::getCreateTables() const {
  return create_tables_;
}

void ConfigList::setCreateTables(bool create_tables) {
  create_tables_ = create_tables;
}

std::map<TableIDType, TableConfig>& ConfigList::getTableConfigs() {
  return table_configs_;
}

const std::map<TableIDType, TableConfig>& ConfigList::getTableConfigs()
    const {
  return table_configs_;
}

const TableConfig* ConfigList::getTableConfig(TableIDType table_id) const {
  auto table_config = table_configs_.find(table_id);
  if (table_config == table_configs_.end()) {
    return nullptr;
  } else {
    return &table_config->second;
  }
}

void ConfigList::addTableConfig(TableConfig config) {
  table_configs_.emplace(config.table_id, std::move(config));
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

const std::map<std::string, std::unique_ptr<IngestionTaskConfig>>& ConfigList::getIngestionTaskConfigs()
    const {
  return ingestion_configs_;
}

const IngestionTaskConfig* ConfigList::getIngestionTaskConfig(std::string sensor_id) const {
  auto ingestion_config = ingestion_configs_.find(sensor_id);
  if (ingestion_config == ingestion_configs_.end()) {
    return nullptr;
  } else {
    return ingestion_config->second.get();
  }
}

void ConfigList::addIngestionTaskConfig(std::unique_ptr<IngestionTaskConfig> config) {
  auto sensor_id = config->sensor_id;
  ingestion_configs_.emplace(sensor_id, std::move(config));
}

} // namespace fnordmetric

