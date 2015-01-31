/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/net/dnscache.h"

namespace fnord {
namespace net {

InetAddr DNSCache::resolve(const std::string& addr_str) {
  std::unique_lock<std::mutex> lk(mutex_);

  auto iter = cache_.find(addr_str);
  if (iter == cache_.end()) {
    lk.unlock();
    auto addr = InetAddr::resolve(addr_str);
    lk.lock();
    cache_.emplace(addr_str, addr);
    return addr;
  } else {
    return iter->second;
  }
}

}
}
