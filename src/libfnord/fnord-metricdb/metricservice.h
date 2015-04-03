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
#include "fnord-base/datetime.h"
#include "fnord-metricdb/metricrepository.h"
#include "fnord-base/stats/counter.h"
#include "fnord-base/thread/taskscheduler.h"

namespace fnord {
namespace metric_service {

class MetricService {
public:

  static MetricService newWithBackend(IMetricRepository* backend);

  static MetricService newWithInMemoryBackend();

  static MetricService newWithDiskBackend(
      const std::string& datadir_path,
      fnord::TaskScheduler* scheduler);

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


  Sample getMostRecentSample(const std::string& metric_key);

  IMetricRepository* metricRepository() const;

protected:
  std::unique_ptr<IMetricRepository> metric_repo_;
};

} // namespace metric_service
} // namsepace fnord
#endif
