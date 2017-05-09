/**
 * This file is part of the "FnordTable" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordTable is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/table_map.h>

namespace fnordmetric {

TableMap::~TableMap() {}

std::shared_ptr<TableConfig> TableMap::findTable(const std::string& key) const {
  auto iter = tables_.find(key);
  if (iter == tables_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

TableMapBuilder::TableMapBuilder() : table_map_(std::make_shared<TableMap>()) {}


std::shared_ptr<TableConfig> TableMapBuilder::findTable(
    const std::string& key) {
  return table_map_->findTable(key);
}

void TableMapBuilder::addTable(
    const TableIDType& key,
    const TableConfig& config) {
  table_map_->tables_.emplace(key, std::make_shared<TableConfig>(config));
}

std::shared_ptr<TableMap> TableMapBuilder::getTableMap() {
  auto mmap = std::move(table_map_);
  table_map_ = std::make_shared<TableMap>();
  return mmap;
}

VersionedTableMap::VersionedTableMap() : table_map_(new TableMap()) {}

std::shared_ptr<TableMap> VersionedTableMap::getTableMap() const {
  std::unique_lock<std::mutex> lk(mutex_);
  return table_map_;
}

void VersionedTableMap::updateTableMap(
    std::shared_ptr<TableMap> table_map) {
  std::unique_lock<std::mutex> lk(mutex_);
  table_map_ = std::move(table_map);
}

} // namespace fnordmetric

