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
#include <metricd/sample.h>
#include <metricd/util/return_code.h>
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
class SeriesIDProvider;

using MetricIDType = std::string;
using SeriesIDType = uint64_t;

struct MetricConfig {
  MetricConfig();
  bool is_valid;
};

class MetricSeries {
public:

  MetricSeries(
      SeriesIDType series_id,
      LabelSet labels);

  ReturnCode insertSample(
      tsdb::TSDB* tsdb,
      Sample sample);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  const LabelSet* getLabels() const;
  bool hasLabel(const std::string& label) const;
  std::string getLabel(const std::string& label) const;
  bool compareLabel(const std::string& label, const std::string& value) const;

protected:
  const SeriesIDType series_id_;
  const LabelSet labels_;
};

struct MetricSeriesMetadata {
  MetricIDType metric_id;
  LabelSet labels;
  bool encode(std::ostream* os) const;
  bool decode(std::istream* is);
};

class MetricSeriesList {
public:

  MetricSeriesList();

  bool findSeries(
      SeriesIDType series_id,
      std::shared_ptr<MetricSeries>* series);

  ReturnCode findOrCreateSeries(
      tsdb::TSDB* tsdb,
      SeriesIDProvider* series_id_provider,
      const std::string& metric_id,
      const LabelSet& labels,
      std::shared_ptr<MetricSeries>* series);

  void addSeries(
      const SeriesIDType& series_id,
      const LabelSet& labels);

  void listSeries(std::set<SeriesIDType>* series_ids);

  size_t getSize() const;

protected:
  mutable std::mutex series_mutex_;
  std::map<SeriesIDType, std::shared_ptr<MetricSeries>> series_;
};

class Metric {
public:

  Metric(
      const std::string& key);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  void setConfig(MetricConfig config);

  MetricSeriesList* getSeriesList();

protected:
  MetricSeriesList series_;
  MetricConfig config_;
  std::mutex config_mutex_;
};

} // namespace fnordmetric
