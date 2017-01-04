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
friend class MetricListCursor;
public:

  ~MetricMap();

  Metric* findMetric(const std::string& key) const;

  std::set<std::string> listMetrics() const;

protected:

  using SlotType = std::pair<Metric*, bool>;
  using MapType = std::map<std::string, SlotType>;
  using IterType = MapType::iterator;

  MapType metrics_;
  std::shared_ptr<MetricMap> next_;
};

class MetricMapBuilder {
public:

  MetricMapBuilder(MetricMap* metric_map);

  void addMetric(
      const std::string& key,
      std::unique_ptr<Metric> metric);

  Metric* findMetric(const std::string& key);

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

class MetricListCursor {
public:

  MetricListCursor(std::shared_ptr<MetricMap> metric_map);
  MetricListCursor(const MetricListCursor& o) = delete;
  MetricListCursor(MetricListCursor&& o);
  MetricListCursor& operator=(const MetricListCursor& o) = delete;

  const std::string& getMetricID();

  bool isValid() const;
  bool next();

protected:
  std::shared_ptr<MetricMap> metric_map_;
  MetricMap::IterType begin_;
  MetricMap::IterType cur_;
  MetricMap::IterType end_;
};

class SeriesIDProvider {
public:

  SeriesIDProvider(SeriesIDType init_id);
  SeriesIDType allocateSeriesID();

protected:
  std::atomic<uint64_t> series_id_;
};

} // namespace fnordmetric

