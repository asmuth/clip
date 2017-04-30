/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metric_map.h>

namespace fnordmetric {

MetricMap::~MetricMap() {}

std::shared_ptr<MetricConfig> MetricMap::findMetric(const std::string& key) const {
  auto iter = tables_.find(key);
  if (iter == tables_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

MetricMapBuilder::MetricMapBuilder() : table_map_(std::make_shared<MetricMap>()) {}


std::shared_ptr<MetricConfig> MetricMapBuilder::findMetric(
    const std::string& key) {
  return table_map_->findMetric(key);
}

void MetricMapBuilder::addMetric(
    const MetricIDType& key,
    const MetricConfig& config) {
  table_map_->tables_.emplace(key, std::make_shared<MetricConfig>(config));
}

std::shared_ptr<MetricMap> MetricMapBuilder::getMetricMap() {
  auto mmap = std::move(table_map_);
  table_map_ = std::make_shared<MetricMap>();
  return mmap;
}

VersionedMetricMap::VersionedMetricMap() : table_map_(new MetricMap()) {}

std::shared_ptr<MetricMap> VersionedMetricMap::getMetricMap() const {
  std::unique_lock<std::mutex> lk(mutex_);
  return table_map_;
}

void VersionedMetricMap::updateMetricMap(
    std::shared_ptr<MetricMap> table_map) {
  std::unique_lock<std::mutex> lk(mutex_);
  table_map_ = std::move(table_map);
}


} // namespace fnordmetric

