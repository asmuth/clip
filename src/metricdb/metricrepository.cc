/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metricrepository.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

MetricRepository::MetricRepository(
    std::shared_ptr<io::FileRepository> file_repo) :
    file_repo_(file_repo) {}

// FIXPAUL lock
Metric* MetricRepository::findMetric(const std::string& key) const {
  Metric* metric = nullptr;

  std::lock_guard<std::mutex> lock_holder(metrics_mutex_);

  auto iter = metrics_.find(key);
  if (iter != metrics_.end()) {
    metric = iter->second.get();
  }

  return metric;
}

Metric* MetricRepository::findOrCreateMetric(const std::string& key) {
  Metric* metric;

  std::lock_guard<std::mutex> lock_holder(metrics_mutex_);

  auto iter = metrics_.find(key);
  if (iter == metrics_.end()) {
    // FIXPAUL expensive operation; should be done outside of lock..
    metric = new Metric(key, file_repo_.get());
    metrics_.emplace(key, std::unique_ptr<Metric>(metric));
  } else {
    metric = iter->second.get();
  }

  return metric;
}

std::vector<std::pair<std::string, Metric*>> MetricRepository::listMetrics()
    const {
  std::vector<std::pair<std::string, Metric*>> metrics;

  {
    std::lock_guard<std::mutex> lock_holder(metrics_mutex_);
    for (const auto& iter : metrics_) {
      metrics.emplace_back(iter.first, iter.second.get());
    }
  }

  return metrics;
}

}
}
