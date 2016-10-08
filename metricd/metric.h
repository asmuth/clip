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
#include <functional>
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

class MetricSeries {
public:

  MetricSeries(
      SeriesIDType series_id,
      LabelSet labels);

  ReturnCode insertSample(Sample sample);

  size_t getTotalBytes() const;
  TimestampType getLastInsertTime();

  const LabelSet* getLabels() const;
  bool hasLabel(const std::string& label) const;
  std::string getLabel(const std::string& label) const;
  bool compareLabel(const std::string& label, const std::string& value) const;

protected:
  const LabelSet labels_;
};

class MetricSeriesList {
public:

  MetricSeriesList();

  bool findSeries(
      SeriesIDType series_id,
      std::shared_ptr<MetricSeries>* series);

  ReturnCode findOrCreateSeries(
      SeriesIDProvider* series_id_provider,
      const LabelSet& labels,
      std::shared_ptr<MetricSeries>* series);

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

  std::set<std::string> getLabels() const;
  bool hasLabel(const std::string& label) const;

  MetricSeriesList* getSeriesList();

protected:
  MetricSeriesList series_;
};

} // namespace fnordmetric

