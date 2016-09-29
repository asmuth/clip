/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_INMEMORY_BACKEND_METRIC_H_
#define _FNORDMETRIC_METRICDB_INMEMORY_BACKEND_METRIC_H_
#include <atomic>
#include <mutex>
#include <metricd/metric.h>

namespace fnord {
namespace metric_service {
namespace inmemory_backend {

using fnord::DateTime;

/**
 * should only be used for debugging purposes
 */
class Metric : public fnord::metric_service::IMetric {
public:
  Metric(const std::string& key);

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

  struct MemSample {
    DateTime time;
    double value;
    std::vector<std::pair<std::string, std::string>> labels;
  };

  const std::string key_;

  mutable std::mutex labels_mutex_;
  std::set<std::string> labels_;
  mutable std::mutex values_mutex_;
  std::vector<MemSample> values_;
  std::atomic_size_t total_bytes_;
  std::atomic_uint_fast64_t last_insert_time_;
};

}
}
}
#endif
