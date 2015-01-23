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
#include <thread>
#include <unistd.h>

namespace fnord {
namespace ping_service {

PingService::PingService() {}

std::string PingService::ping(std::string echo) {
  return StringUtil::format("pong: $0", echo);
}

void PingService::ping_async(
    fnord::RPC<std::string, std::tuple<std::string>>* rpc) {
  auto res = StringUtil::format("pong: $0", std::get<0>(rpc->args()));

  auto t = std::thread([rpc, res] () {
    usleep(1000000);
    rpc->ready(res);
  });

  t.detach();
}

} // namespace ping_service
} // namespace fnord
