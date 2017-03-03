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
#include <fnordmetric/table_map.h>

namespace fnordmetric {

TableMap::~TableMap() {
  for (auto& m : tables_) {
    if (m.second.second) {
      delete m.second.first;
    }
  }
}

TableConfig* TableMap::findTable(const std::string& key) const {
  auto iter = tables_.find(key);
  if (iter == tables_.end()) {
    return nullptr;
  } else {
    return iter->second.first;
  }
}

TableMapBuilder::TableMapBuilder(
    TableMap* mm) :
    metric_map_(std::make_shared<TableMap>()) {
  if (mm) {
    for (auto& m : mm->tables_) {
      m.second.second = false;

      metric_map_->tables_.emplace(
          m.first,
          std::make_pair(m.second.first, true));
    }

    mm->next_ = metric_map_;
  }
}

TableConfig* TableMapBuilder::findTable(
    const std::string& key) {
  return metric_map_->findTable(key);
}

void TableMapBuilder::addTable(
    const TableIDType& key,
    const TableConfig& config) {
  metric_map_->tables_.emplace(key, std::make_pair(new TableConfig(config), true));
}

std::shared_ptr<TableMap> TableMapBuilder::getTableMap() {
  auto mmap = std::move(metric_map_);
  metric_map_ = std::make_shared<TableMap>();
  return mmap;
}

VersionedTableMap::VersionedTableMap() : metric_map_(new TableMap()) {}

std::shared_ptr<TableMap> VersionedTableMap::getTableMap() const {
  std::unique_lock<std::mutex> lk(mutex_);
  return metric_map_;
}

void VersionedTableMap::updateTableMap(
    std::shared_ptr<TableMap> metric_map) {
  std::unique_lock<std::mutex> lk(mutex_);
  metric_map_ = std::move(metric_map);
}

} // namespace fnordmetric

