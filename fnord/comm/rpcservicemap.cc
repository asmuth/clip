/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-rpc/RPC.h"
#include "fnord/comm/rpcservicemap.h"

namespace fnord {
namespace comm {

RPCChannel* RPCServiceMap::getChannel(const std::string& service_name) {
  auto iter = channels_.find(service_name);

  if (iter == channels_.end()) {
    RAISEF(kRPCError, "no such service: $0", service_name);
  }

  return iter->second.get();
}

void RPCServiceMap::addChannel(
    const std::string& service_name,
    std::unique_ptr<RPCChannel>&& channel) {
  channels_[service_name] = std::move(channel);
}


} // namespace comm
} // namsepace fnord

