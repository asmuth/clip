/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_DISK_BACKEND_METRICREPOSITORY_H_
#define _FNORDMETRIC_METRICDB_DISK_BACKEND_METRICREPOSITORY_H_
#include <fnord-metricdb/backends/disk/compactiontask.h>
#include <fnord-metricdb/backends/disk/metric.h>
#include <fnord-metricdb/metricrepository.h>
#include <fnord-base/io/filerepository.h>
#include <fnord-base/thread/taskscheduler.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

class MetricRepository : public fnord::metric_service::IMetricRepository {
public:
  MetricRepository(
      const std::string data_dir,
      fnord::TaskScheduler* scheduler);

protected:
  Metric* createMetric(const std::string& key) override;
  std::shared_ptr<fnord::FileRepository> file_repo_;
  CompactionTask compaction_task_;
};

}
}
}
#endif
