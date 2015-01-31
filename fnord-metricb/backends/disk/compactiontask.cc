/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <fnord/base/wallclock.h>
#include <fnord/service/metric/backends/disk/compactiontask.h>
#include <fnord/service/metric/backends/disk/metricrepository.h>

using fnord::WallClock;

namespace fnord {
namespace metric_service {
namespace disk_backend {

CompactionTask::CompactionTask(
    MetricRepository* metric_repo) :
    metric_repo_(metric_repo),
    run_every_micros_(kRunEveryMicrosDefault) {}

std::function<void()> CompactionTask::runnable() const {
  return [this] () -> void { run(); };
}

void CompactionTask::run() const {
  auto last_run = WallClock::unixMicros();

  for (;;) {
    auto next_run = last_run + run_every_micros_;
    auto now = WallClock::unixMicros();
    last_run = now;

    if (now < next_run) {
      usleep(next_run - now);
    }

    for (const auto& metric : metric_repo_->listMetrics()) {
      try {
        auto disk_metric = dynamic_cast<Metric*>(metric);

        if (disk_metric != nullptr) {
          disk_metric->compact();
        }
      } catch (fnord::Exception e) {
        //env()->logger()->printf(
        //    "ERROR",
        //    "uncaught exception while executing Metric#compact(): %s\n");

        e.debugPrint();
      }
    }
  }
}

}
}
}

