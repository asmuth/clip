/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/backends/inmemory/metricrepository.h"

namespace fnord {
namespace metric_service {

MetricService MetricService::newWithInMemoryBackend() {
  std::unique_ptr<IMetricRepository> metric_repo(
      new inmemory_backend::MetricRepository());

  return MetricService(std::move(metric_repo));
}

//MetricService MetricService::newWithDiskBackend() {
//}

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

}

void MetricService::scanSamples(
    const std::string& metric_key,
    const fnord::DateTime& time_begin,
    const fnord::DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
}


} // namespace metric_service
} // namsepace fnord

