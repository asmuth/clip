/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/ping/pingservice.h"
#include "fnord/base/stringutil.h"
#include "fnord/reflect/reflect.h"

namespace fnord {
namespace ping_service {

PingService::PingService() {}

std::string PingService::ping(std::string echo) {
  return StringUtil::format("pong: $0", echo);
}

int PingService::pingint(int i) {
  return i * 3;
}

} // namespace ping_service
} // namespace fnord
