/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/wallclock.h>
#include <fnord/service/metric/backends/inmemory/metric.h>

namespace fnord {
namespace metric_service {
namespace inmemory_backend {

using fnord::DateTime;
using fnord::WallClock;

Metric::Metric(
    const std::string& key) :
    IMetric(key),
    total_bytes_(0),
    last_insert_time_(0) {}

void Metric::insertSampleImpl(
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {

  {
    std::lock_guard<std::mutex> lock_holder(labels_mutex_);
    for (const auto& pair : labels) {
      labels_.emplace(pair.first);
    }
  }

  {
    std::lock_guard<std::mutex> lock_holder(values_mutex_);
    last_insert_time_ = WallClock::unixMicros();
    MemSample sample = {
      .time = DateTime(last_insert_time_),
      .value = value,
      .labels = labels};

    values_.emplace_back(sample);
  }
}

void Metric::scanSamples(
    const DateTime& time_begin,
    const DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
  std::lock_guard<std::mutex> lock_holder(values_mutex_);

  for (const auto sample : values_) {
    if (sample.time < time_begin) {
      continue;
    }

    if (sample.time >= time_end) {
      break;
    }

    Sample cb_sample(sample.time, sample.value, sample.labels);
    if (!callback(&cb_sample)) {
      break;
    }
  }
}

size_t Metric::totalBytes() const {
  return total_bytes_;
}

DateTime Metric::lastInsertTime() const {
  return DateTime(last_insert_time_);
}

std::set<std::string> Metric::labels() const {
  std::lock_guard<std::mutex> lock_holder(labels_mutex_);
  return labels_;
}

bool Metric::hasLabel(const std::string& label) const {
  std::lock_guard<std::mutex> lock_holder(labels_mutex_);
  return labels_.find(label) != labels_.end();
}

}
}
}

