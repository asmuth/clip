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

using MetricIDType = std::string;
using SeriesIDType = uint64_t;

enum class MetricAggregationType {
  NONE,
  RANDOM,
  LATEST,
  NEWEST,
  MIN,
  MAX,
  AVG,
  SUM,
  SUMRATE,
  RATE
};

struct MetricConfig {
  MetricConfig();
  MetricDataType data_type;
  MetricAggregationType aggregation;
  uint64_t granularity;
  uint64_t display_granularity;
  bool is_valid;
};

class MetricSeries {
public:

  MetricSeries(
      SeriesIDType series_id,
      std::string series_name);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  SeriesIDType getSeriesID() const;
  const std::string& getSeriesName() const;

protected:
  const SeriesIDType series_id_;
  std::string series_name_;
};

struct MetricSeriesMetadata {
  MetricIDType metric_id;
  std::string series_name;
  bool encode(std::ostream* os) const;
  bool decode(std::istream* is);
};

class MetricSeriesList {
public:

  MetricSeriesList();

  bool findSeriesByID(
      SeriesIDType series_id,
      std::shared_ptr<MetricSeries>* series);

  bool findSeriesByName(
      const std::string& name,
      std::shared_ptr<MetricSeries>* series);

  ReturnCode findOrCreateSeriesByName(
      tsdb::TSDB* tsdb,
      SeriesIDProvider* series_id_provider,
      const std::string& metric_id,
      const MetricConfig& config,
      const std::string& series_name,
      std::shared_ptr<MetricSeries>* series);

  void addSeries(
      const SeriesIDType& series_id,
      const std::string& series_name);

  void listSeries(std::vector<SeriesIDType>* series_ids);

  size_t getSize() const;

protected:
  mutable std::mutex series_mutex_;
  std::map<std::string, std::shared_ptr<MetricSeries>> series_;
  std::map<SeriesIDType, std::shared_ptr<MetricSeries>> series_by_id_;
};

class MetricSeriesCursor {
public:

  MetricSeriesCursor();
  MetricSeriesCursor(const MetricConfig* config, tsdb::Cursor cursor);

  MetricSeriesCursor(const MetricSeriesCursor& o) = delete;
  MetricSeriesCursor(MetricSeriesCursor&& o);
  MetricSeriesCursor& operator=(const MetricSeriesCursor& o) = delete;
  MetricSeriesCursor& operator=(MetricSeriesCursor&& o);

  bool next(uint64_t* timestamp, uint64_t* value);

protected:

  tsdb::Cursor cursor_;
  std::unique_ptr<OutputAggregator> aggr_;
};

class MetricSeriesListCursor {
public:

  using ListType = std::vector<SeriesIDType>;
  using ListIterType = ListType::iterator;

  MetricSeriesListCursor();
  MetricSeriesListCursor(
      std::shared_ptr<MetricMap> metric_map,
      MetricSeriesList* series_list,
      ListType&& snapshot);

  MetricSeriesListCursor(const MetricSeriesListCursor& o) = delete;
  MetricSeriesListCursor(MetricSeriesListCursor&& o);
  MetricSeriesListCursor& operator=(const MetricSeriesListCursor& o) = delete;
  MetricSeriesListCursor& operator=(MetricSeriesListCursor&& o);

  SeriesIDType getSeriesID() const;
  const std::string& getSeriesName() const;

  bool isValid() const;
  bool next();

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

  const MetricConfig& getConfig() const;
  void setConfig(MetricConfig config);

  MetricSeriesList* getSeriesList();

  InputAggregator* getInputAggregator();

protected:
  std::string key_;
  MetricSeriesList series_;
  MetricConfig config_;
  std::unique_ptr<InputAggregator> input_aggr_;
};

std::unique_ptr<OutputAggregator> mkOutputAggregator(
    tsdb::Cursor* cursor,
    const MetricConfig* config);


std::unique_ptr<InputAggregator> mkInputAggregator(
    const MetricConfig* config);

} // namespace fnordmetric

