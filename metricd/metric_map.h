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
#include <metricd/metric.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace fnordmetric {

class MetricMap {
friend class MetricMapBuilder;
public:

  virtual ~MetricMap() {}

  Metric* findMetric(const std::string& key) const;

  std::set<std::string> listMetrics() const;

protected:
  std::map<std::string, Metric*> metrics_;
};

class MetricMapBuilder {
public:

  MetricMapBuilder();

  void copyFrom(const MetricMap* metric_map);

  void addMetric(
      const std::string& key,
      std::unique_ptr<Metric> metric);

  std::shared_ptr<MetricMap> getMetricMap();

protected:
  std::shared_ptr<MetricMap> metric_map_;
};

class VersionedMetricMap {
public:

  std::shared_ptr<MetricMap> getMetricMap();

  void updateMetricMap(std::shared_ptr<MetricMap> metric_map);

protected:
  std::mutex mutex_;
  std::shared_ptr<MetricMap> metric_map_;
};

class SeriesIDProvider {
public:

  SeriesIDProvider(SeriesIDType init_id);
  SeriesIDType allocateSeriesID();

protected:
  std::atomic<SeriesIDType> series_id_;
};

} // namespace fnordmetric

