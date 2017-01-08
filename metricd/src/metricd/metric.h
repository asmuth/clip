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
#include <metricd/types.h>
#include <metricd/sample.h>
#include <metricd/util/return_code.h>
#include <metricd/aggregate.h>
#include <metricd/units.h>
#include <libtsdb/tsdb.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>

namespace fnordmetric {
class MetricMap;
class SeriesIDProvider;
class MetricSeriesListCursor;

using MetricIDType = std::string;

struct SeriesIDType {
  explicit SeriesIDType() : id(0) {}
  explicit SeriesIDType(uint64_t id_) : id(id_) {}
  uint64_t id;
};

struct SeriesNameType {
  SeriesNameType() = default;
  explicit SeriesNameType(const std::string& name_) : name(name_) {}
  std::string name;
};

enum MetricKind : uint32_t {
  UNKNOWN           = 0,
  SAMPLE_UINT64     = 1,
  SAMPLE_INT64      = 2,
  SAMPLE_FLOAT64    = 3,
  COUNTER_UINT64    = 4,
  COUNTER_INT64     = 5,
  COUNTER_FLOAT64   = 6,
  MONOTONIC_UINT64  = 7,
  MONOTONIC_INT64   = 8,
  MONOTONIC_FLOAT64 = 9,
  MIN_UINT64        = 10,
  MIN_INT64         = 11,
  MIN_FLOAT64       = 12,
  MAX_UINT64        = 13,
  MAX_INT64         = 14,
  MAX_FLOAT64       = 15,
  AVERAGE_UINT64    = 16,
  AVERAGE_INT64     = 17,
  AVERAGE_FLOAT64   = 18
};

struct MetricConfig {
  MetricConfig();
  MetricKind kind;
  uint64_t granularity;
  uint64_t display_granularity;
  bool is_valid;
  std::string unit_id;
  tval_autoref unit_scale;
};

class MetricSeries {
public:

  MetricSeries(
      SeriesIDType series_id,
      SeriesNameType series_name);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  SeriesIDType getSeriesID() const;
  const SeriesNameType& getSeriesName() const;

protected:
  const SeriesIDType series_id_;
  SeriesNameType series_name_;
};

struct MetricSeriesMetadata {
  MetricIDType metric_id;
  MetricKind metric_kind;
  SeriesNameType series_name;
  bool encode(std::ostream* os) const;
  bool decode(std::istream* is);
};

class MetricSeriesList {
public:

  MetricSeriesList();

  bool findSeries(
      const SeriesIDType& series_id,
      std::shared_ptr<MetricSeries>* series);

  bool findSeries(
      const SeriesNameType& series_name,
      std::shared_ptr<MetricSeries>* series);

  ReturnCode findOrCreateSeries(
      tsdb::TSDB* tsdb,
      SeriesIDProvider* series_id_provider,
      const std::string& metric_id,
      const MetricConfig* config,
      const SeriesNameType& series_name,
      std::shared_ptr<MetricSeries>* series);

  void addSeries(
      const SeriesIDType& series_id,
      const SeriesNameType& series_name);

  void listSeries(std::vector<SeriesIDType>* series_ids);
  MetricSeriesListCursor listSeries();

  size_t getSize() const;

protected:
  mutable std::mutex series_mutex_;
  std::map<std::string, std::shared_ptr<MetricSeries>> series_;
  std::map<uint64_t, std::shared_ptr<MetricSeries>> series_by_id_;
};

class MetricSeriesListCursor {
public:

  using ListType = std::vector<SeriesIDType>;
  using ListIterType = ListType::iterator;

  MetricSeriesListCursor();
  MetricSeriesListCursor(
      MetricSeriesList* series_list,
      ListType&& snapshot);

  MetricSeriesListCursor(const MetricSeriesListCursor& o) = delete;
  MetricSeriesListCursor(MetricSeriesListCursor&& o);
  MetricSeriesListCursor& operator=(const MetricSeriesListCursor& o) = delete;
  MetricSeriesListCursor& operator=(MetricSeriesListCursor&& o);

  SeriesIDType getSeriesID() const;
  const SeriesNameType& getSeriesName() const;

  bool isValid() const;
  bool next();

  void setMetricMap(std::shared_ptr<MetricMap> metric_map);

protected:

  bool fetchNext();

  bool valid_;
  std::shared_ptr<MetricMap> metric_map_;
  MetricSeriesList* series_list_;
  ListType snapshot_;
  ListIterType cursor_;
  std::shared_ptr<MetricSeries> series_;
};

class Metric {
public:

  Metric(
      const std::string& key);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  const MetricConfig* getConfig() const;
  ReturnCode setConfig(const MetricConfig* config);

  MetricSeriesList* getSeriesList();

  InputAggregator* getInputAggregator();

  void setUnitConfig(const UnitConfig* config);
  const UnitConfig* getUnitConfig() const;

protected:
  std::string key_;
  MetricSeriesList series_;
  const MetricConfig* config_;
  std::unique_ptr<InputAggregator> input_aggr_;
  const UnitConfig* unit_config_;
};

class MetricInfo {
public:

  MetricInfo();
  MetricInfo(Metric* metric, std::shared_ptr<MetricMap> metric_map);

  MetricInfo(const MetricInfo& o) = delete;
  MetricInfo(MetricInfo&& o);
  MetricInfo& operator=(const MetricInfo& o) = delete;
  MetricInfo& operator=(MetricInfo&& o);

  const UnitConfig* getUnitConfig() const;
  const MetricConfig* getMetricConfig() const;

protected:
  Metric* metric_;
  std::shared_ptr<MetricMap> metric_map_;
};

tval_type getMetricDataType(MetricKind t);

} // namespace fnordmetric

