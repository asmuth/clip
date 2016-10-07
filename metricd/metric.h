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
#include <functional>
#include <string>
#include <vector>
#include <set>
#include <memory>

namespace fnordmetric {

class Metric {
public:

  using SeriesIDType = uint64_t;
  using TimestampType = Sample::TimestampType;

  Metric(const std::string& key);

  virtual size_t getTotalBytes() const;
  virtual Metric::TimestampType getLastInsertTime();

  virtual std::set<std::string> getLabels() const;
  virtual bool hasLabel(const std::string& label) const;

};

class MetricSeries : std::enable_shared_from_this<MetricSeries> {
public:

  void insertSample(Sample sample);

  virtual size_t getTotalBytes() const;
  virtual Metric::TimestampType getLastInsertTime();

  virtual std::set<std::string> getLabels() const;
  virtual bool hasLabel(const std::string& label) const;
  virtual bool getLabel(const std::string& label) const;

protected:

  const std::string key_;
};

class MetricSeriesList {
public:

  using LabelSet = std::vector<std::pair<std::string, std::string>>;

  bool findSeries(
      Metric::SeriesIDType series_id,
      std::shared_ptr<MetricSeries>* series);

  bool findOrCreateSeries(
      LabelSet labels,
      std::shared_ptr<MetricSeries>* series);

  void listSeries(std::set<Metric::SeriesIDType>* series_ids);

  size_t getSize() const;

protected:
  mutable std::mutex series_mutex_;
  std::map<Metric::SeriesIDType, std::shared_ptr<MetricSeries>> series_;
};

} // namespace fnordmetric

