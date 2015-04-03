/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-metricdb/metricservice.h"
#include "fnord-metricdb/backends/disk/metricrepository.h"
#include "fnord-metricdb/backends/inmemory/metricrepository.h"

namespace fnord {
namespace metric_service {

MetricService MetricService::newWithInMemoryBackend() {
  return newWithBackend(new inmemory_backend::MetricRepository());
}

MetricService MetricService::newWithDiskBackend(
    const std::string& datadir_path,
    fnord::TaskScheduler* scheduler) {
  return newWithBackend(
      new disk_backend::MetricRepository(datadir_path, scheduler));
}

MetricService MetricService::newWithBackend(IMetricRepository* metric_repo) {
  return MetricService(
      std::move(std::unique_ptr<IMetricRepository>(metric_repo)));
}

MetricService::MetricService(
    std::unique_ptr<IMetricRepository> metric_repo) :
    metric_repo_(std::move(metric_repo)) {}

MetricService::MetricService(
    MetricService&& other) :
    metric_repo_(std::move(other.metric_repo_)) {}

std::vector<IMetric*> MetricService::listMetrics() const {
  return metric_repo_->listMetrics();
}

void MetricService::insertSample(
    const std::string& metric_key,
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  auto metric = metric_repo_->findOrCreateMetric(metric_key);
  metric->insertSample(value, labels);
}

void MetricService::scanSamples(
    const std::string& metric_key,
    const fnord::DateTime& time_begin,
    const fnord::DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
  auto metric = metric_repo_->findMetric(metric_key);

  if (metric != nullptr) {
    metric->scanSamples(time_begin, time_end, callback);
  }
}

Sample MetricService::getMostRecentSample(const std::string& metric_key) {
  auto metric = metric_repo_->findMetric(metric_key);

  if (metric == nullptr) {
    RAISEF(kIndexError, "unknown metric: $0", metric_key);
  }

  return metric->getSample();
}

IMetricRepository* MetricService::metricRepository() const {
  return metric_repo_.get();
}

} // namespace metric_service
} // namsepace fnord

