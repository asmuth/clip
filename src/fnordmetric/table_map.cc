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
  for (auto& m : metrics_) {
    if (m.second.second) {
      delete m.second.first;
    }
  }
}

TableConfig* TableMap::findTable(const std::string& key) const {
  auto iter = metrics_.find(key);
  if (iter == metrics_.end()) {
    return nullptr;
  } else {
    return iter->second.first;
  }
}

//std::vector<ITableConfig*> ITableMap::listTables()
//    const {
//  std::vector<ITableConfig*> metrics;
//
//  {
//    std::lock_guard<std::mutex> lock_holder(metrics_mutex_);
//    for (const auto& iter : metrics_) {
//      metrics.emplace_back(iter.second.get());
//    }
//  }
//
//  return metrics;
//}

TableMapBuilder::TableMapBuilder(
    TableMap* mm) :
    metric_map_(std::make_shared<TableMap>()) {
  if (mm) {
    for (auto& m : mm->metrics_) {
      m.second.second = false;

      metric_map_->metrics_.emplace(
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
    const std::string& key,
    std::unique_ptr<TableConfig> metric) {
  metric_map_->metrics_.emplace(key, std::make_pair(metric.release(), true));
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

TableListCursor::TableListCursor(
    std::shared_ptr<TableMap> metric_map) :
    metric_map_(std::move(metric_map)),
    begin_(metric_map_->metrics_.begin()),
    cur_(begin_),
    end_(metric_map_->metrics_.end()) {}

TableListCursor::TableListCursor(
    TableListCursor&& o) :
    metric_map_(std::move(o.metric_map_)),
    begin_(std::move(o.begin_)),
    cur_(std::move(o.cur_)),
    end_(std::move(o.end_)) {}

const std::string& TableListCursor::getTableID() {
  return cur_->first;
}

bool TableListCursor::isValid() const {
  return cur_ != end_;
}

bool TableListCursor::next() {
  if (cur_ == end_) {
    return false;
  } else {
    ++cur_;
    return true;
  }
}

} // namespace fnordmetric

