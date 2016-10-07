/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/metricrepository.h>

using namespace fnord;
namespace fnord {
namespace metric_service {

IMetric* IMetricRepository::findMetric(const std::string& key) const {
  IMetric* metric = nullptr;

  std::lock_guard<std::mutex> lock_holder(metrics_mutex_);

  auto iter = metrics_.find(key);
  if (iter != metrics_.end()) {
    metric = iter->second.get();
  }

  return metric;
}

IMetric* IMetricRepository::findOrCreateMetric(const std::string& key) {
  IMetric* metric;
  std::lock_guard<std::mutex> lock_holder(metrics_mutex_);

  auto iter = metrics_.find(key);
  if (iter == metrics_.end()) {
    /*if (env()->verbose()) {
      env()->logger()->printf(
          "DEBUG",
          "Create new metric: '%s'",
          key.c_str());
    }*/

    // FIXPAUL expensive operation; should be done outside of lock..
    metric = createMetric(key);
    metrics_.emplace(key, std::unique_ptr<IMetric>(metric));
  } else {
    metric = iter->second.get();
  }

  return metric;
}

std::vector<IMetric*> IMetricRepository::listMetrics()
    const {
  std::vector<IMetric*> metrics;

  {
    std::lock_guard<std::mutex> lock_holder(metrics_mutex_);
    for (const auto& iter : metrics_) {
      metrics.emplace_back(iter.second.get());
    }
  }

  return metrics;
}

}
}
