/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/sensor_http.h>
#include <metricd/util/time.h>
#include <iostream>

namespace fnordmetric {

HTTPSensorTask::HTTPSensorTask() : next_invocation_(0) {}

uint64_t HTTPSensorTask::getNextInvocationTime() const {
  return next_invocation_;
}

ReturnCode HTTPSensorTask::invoke() {
  std::cerr << "HTTPSensorTask::invoke() called" << std::endl;
  next_invocation_ = MonotonicClock::now() + 1 * kMicrosPerSecond;
  return ReturnCode::success();
}

} // namespace fnordmetric

