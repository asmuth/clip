/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_COMPACTIONTASK_H_
#define _FNORDMETRIC_METRICDB_COMPACTIONTASK_H_
#include <functional>

namespace fnordmetric {
namespace metricdb {
class MetricRepository;

class CompactionTask {
public:
  static const uint64_t kRunEveryMicrosDefault = 30 * 1000000;

  CompactionTask(MetricRepository* metric_repo);
  std::function<void()> runnable() const;
protected:
  void run() const;
  MetricRepository* metric_repo_;
  uint64_t run_every_micros_;
};


}
}
#endif
