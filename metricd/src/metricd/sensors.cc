/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/sensors.h>

namespace fnordmetric {

ReturnCode mkSensorTask(
    const SensorConfig* sensor_cfg,
    std::unique_ptr<SensorTask>* sensor_task) {
  return ReturnCode::errorf(
      "EARG",
      "invalid sensor type for sensor: $0",
      sensor_cfg->sensor_id);
}

void SensorScheduler::addTask(std::unique_ptr<SensorTask> task) {

}

ReturnCode SensorScheduler::start() {
  return ReturnCode::success();
}

void SensorScheduler::shutdown() {
}

} // namespace fnordmetric

