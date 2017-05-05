/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <unistd.h>
#include <metrictools/collect.h>

namespace fnordmetric {
struct PipeRef;

struct CollectProcTaskConfig : public IngestionTaskConfig {
  CollectProcTaskConfig();
  std::string command;
  uint64_t interval;
  MeasurementCoding format;
};

class CollectProcTask : public PeriodicIngestionTask {
public:

  static ReturnCode start(
      Backend* storage_backend,
      const ConfigList* config,
      const IngestionTaskConfig* task_config,
      std::unique_ptr<IngestionTask>* task);

  CollectProcTask(
      Backend* storage_backend,
      const ConfigList* config,
      uint64_t interval,
      const std::string& cmd_path,
      MeasurementCoding format);

  ReturnCode invoke() override;

protected:

  ReturnCode runProcess(std::string* stdout_buf);

  ReturnCode spawnProcess(
      PipeRef* stdout_pipe,
      PipeRef* stderr_pipe,
      pid_t* pid);

  ReturnCode waitProcess(
      PipeRef* stdout_pipe,
      PipeRef* stderr_pipe,
      std::string* stdout_buf,
      pid_t* pid);

  void logStderr(const std::string& line);

  Backend* storage_backend_;
  const ConfigList* config_;
  std::string cmd_path_;
  MeasurementCoding format_;
};

} // namespace fnordmetric

