/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/comm/lbgroup.h"

namespace fnord {
namespace comm {

LBGroup::LBGroup() {}

net::InetAddr LBGroup::getServerForNextRequest() {
  std::unique_lock<std::mutex> lk(mutex_);

  if (servers_.empty()) {
    RAISE(kRPCError, "lb group is empty, giving up");
  }

  auto picked_index = pickServerForNextRequest(servers_);
  if (picked_index < 0) {
    RAISE(kRPCError, "no available servers for this request, giving up");
  }

  return servers_[picked_index].addr;
}

void LBGroup::addServer(const net::InetAddr& addr) {
  std::unique_lock<std::mutex> lk(mutex_);
  servers_.emplace_back(addr);
}

void LBGroup::removeServer(const net::InetAddr& addr) {
}

void LBGroup::markServerAsDown(const net::InetAddr& addr) {
}

LBGroup::Server::Server(
    const net::InetAddr& _addr) :
    addr(_addr),
    state(S_UP) {}

RoundRobinLBGroup::RoundRobinLBGroup() : last_index_(0) {}

int RoundRobinLBGroup::pickServerForNextRequest(
    const std::vector<Server>& servers) {
  auto s = servers.size();

  for (int i = 0; i < s; ++i) {
    last_index_ = (last_index_ + 1) % s;

    if (servers[last_index_].state == S_UP) {
      return last_index_;
    }
  }

  return -1;
}

}
}
