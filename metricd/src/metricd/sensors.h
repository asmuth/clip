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
#include <metricd/types.h>
#include <metricd/util/return_code.h>

namespace fnordmetric {

struct SensorConfig {
  virtual ~SensorConfig() = default;
  std::string sensor_id;
};

class SensorTask {
public:

  virtual ~SensorTask() = default;

  virtual uint64_t getNextInvocationTime() const = 0;

  virtual ReturnCode invoke() = 0;

};

ReturnCode mkSensorTask(
    const SensorConfig* sensor_cfg,
    std::unique_ptr<SensorTask>* sensor_task);

class SensorScheduler{
public:

  void addTask(std::unique_ptr<SensorTask> task);

  ReturnCode start();
  void shutdown();

};

} // namespace fnordmetric

