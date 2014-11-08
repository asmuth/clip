/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/backends/inmemory/metric.h>

namespace fnordmetric {
namespace metricdb {
namespace inmemory_backend {

using fnord::util::DateTime;

Metric::Metric(const std::string& key) : IMetric(key) {}

void Metric::insertSample(
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  {
    std::lock_guard<std::mutex> lock_holder(labels_mutex_);
    for (const auto& pair : labels) {
      labels_.emplace(pair.first);
    }
  }
}

void Metric::scanSamples(
    const DateTime& time_begin,
    const DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
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

