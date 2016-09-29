/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <mutex>
#include <metricd/metric.h>
#include <tsdb/TSDBClient.h>

namespace fnord {
namespace metric_service {
namespace tsdb_backend {

using fnord::DateTime;

/**
 * should only be used for debugging purposes
 */
class Metric : public fnord::metric_service::IMetric {
public:
  Metric(
      const String& key,
      const String& tsdb_prefix,
      tsdb::TSDBClient* tsdb);

  void scanSamples(
      const DateTime& time_begin,
      const DateTime& time_end,
      std::function<bool (Sample* sample)> callback) override;

  Sample getSample() override;

  size_t totalBytes() const override;
  DateTime lastInsertTime() const override;
  std::set<std::string> labels() const override;
  bool hasLabel(const std::string& label) const override;

protected:
  void insertSampleImpl(
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) override;

  const std::string key_;
  String tsdb_prefix_;
  tsdb::TSDBClient* tsdb_;

  mutable std::mutex labels_mutex_;
  std::set<std::string> labels_;
  std::atomic_size_t total_bytes_;
  std::atomic_uint_fast64_t last_insert_time_;
};

}
}
}
