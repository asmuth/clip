/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRIC_SERVICE_H
#define _FNORD_METRIC_SERVICE_H
#include <mutex>
#include <stdlib.h>
#include <string>
#include "fnord/base/datetime.h"
#include "fnord/service/metric/metricrepository.h"
#include "fnord/stats/counter.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace metric_service {

class MetricService {
public:
  static MetricService newWithInMemoryBackend(
      fnord::logging::Logger* logger,
      fnord::stats::StatsManager* stats);

  static MetricService newWithDiskBackend(
      const std::string& datadir_path,
      fnord::thread::TaskScheduler* scheduler,
      fnord::logging::Logger* logger,
      fnord::stats::StatsManager* stats);

  MetricService(std::unique_ptr<IMetricRepository> metric_repo);
  MetricService(MetricService&& other);
  MetricService(const MetricService& other) = delete;
  MetricService& operator=(const MetricService& other) = delete;

  /**
   * List all metrics
   */
  std::vector<IMetric*> listMetrics() const;

  /**
   * Insert a sample into a metric
   */
  void insertSample(
      const std::string& metric_key,
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels);

  /**
   * Return all samples for the provided metric and time range
   */
  void scanSamples(
      const std::string& metric_key,
      const fnord::DateTime& time_begin,
      const fnord::DateTime& time_end,
      std::function<bool (Sample* sample)> callback);

  IMetricRepository* metricRepository() const;

protected:
  std::unique_ptr<IMetricRepository> metric_repo_;

  stats::Counter<uint64_t> stat_samples_inserted_;
};

} // namespace metric_service
} // namsepace fnord
#endif
