/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/comm/lbgroup.h"

namespace fnord {
namespace comm {

LBGroup::LBGroup() {}

net::InetAddr LBGroup::getServerForNextRequest() {
}

void LBGroup::addServer(const net::InetAddr& addr) {
}

void LBGroup::removeServer(const net::InetAddr& addr) {
}

void LBGroup::markServerAsDown(const net::InetAddr& addr) {
}

size_t RoundRobinLBGroup::pickServerForNextRequest(
    const std::vector<Server>& servers) {
  return 0;
}

}
}
