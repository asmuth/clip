/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/collect.h>
#include <metrictools/collect_proc.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/storage/backend.h>

namespace fnordmetric {

CollectProcTaskConfig::CollectProcTaskConfig() :
    interval(10 * kMicrosPerSecond),
    format(MeasurementCoding::STATSD) {}

ReturnCode CollectProcTask::start(
    Backend* storage_backend,
    const ConfigList* config,
    const IngestionTaskConfig* task_config,
    std::unique_ptr<IngestionTask>* task) {
  auto c = dynamic_cast<const CollectProcTaskConfig*>(task_config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->command.empty()) {
    return ReturnCode::error("ERUNTIME", "missing command");
  }

  task->reset(
      new CollectProcTask(
          storage_backend,
          config,
          c->interval,
          c->command,
          c->format));

  return ReturnCode::success();
}

CollectProcTask::CollectProcTask(
    Backend* storage_backend,
    const ConfigList* config,
    uint64_t interval,
    const std::string& command,
    MeasurementCoding format) :
    PeriodicIngestionTask(interval),
    storage_backend_(storage_backend),
    config_(config),
    command_(command),
    format_(format) {}

ReturnCode CollectProcTask::invoke() {
  logDebug("Running: $0", command_);
  return ReturnCode::success();
}

} // namespace fnordmetric

