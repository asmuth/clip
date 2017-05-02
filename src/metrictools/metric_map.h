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
#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <metrictools/metric.h>

namespace fnordmetric {

class MetricMap {
friend class MetricMapBuilder;
public:

  ~MetricMap();

  std::shared_ptr<MetricConfig> findMetric(const std::string& key) const;

  std::set<std::string> listMetrics() const;

protected:

  using SlotType = std::shared_ptr<MetricConfig>;
  using MapType = std::map<std::string, SlotType>;
  using IterType = MapType::iterator;

  MapType tables_;
};

class MetricMapBuilder {
public:

  MetricMapBuilder();

  void addMetric(
      const MetricIDType& table_id,
      const MetricConfig& table);

  std::shared_ptr<MetricConfig> findMetric(const MetricIDType& table_id);

  std::shared_ptr<MetricMap> getMetricMap();

protected:
  std::shared_ptr<MetricMap> table_map_;
};

class VersionedMetricMap {
public:

  VersionedMetricMap();

  std::shared_ptr<MetricMap> getMetricMap() const;

  void updateMetricMap(std::shared_ptr<MetricMap> table_map);

protected:
  mutable std::mutex mutex_;
  std::shared_ptr<MetricMap> table_map_;
};

} // namespace fnordmetric

