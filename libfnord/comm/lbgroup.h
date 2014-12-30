/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_COMM_LBGROUP_H
#define _FNORD_COMM_LBGROUP_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/net/inetaddr.h"

namespace fnord {
namespace comm {

class LBGroup {
public:
  LBGroup();

  net::InetAddr getServerForNextRequest();

  void addServer(const net::InetAddr& addr);
  void removeServer(const net::InetAddr& addr);
  void markServerAsDown(const net::InetAddr& addr);

protected:
  enum kServerState {
    S_UP,
    S_DOWN
  };

  struct Server {
    Server(const net::InetAddr& addr);
    net::InetAddr addr;
    kServerState state;
  };

  /**
   * Should return the index of the picked host
   */
  virtual int pickServerForNextRequest(
      const std::vector<Server>& servers) = 0;

private:
  std::mutex mutex_;
  std::vector<Server> servers_;
};

class RoundRobinLBGroup : public LBGroup {
public:
  RoundRobinLBGroup();
protected:
  int pickServerForNextRequest(const std::vector<Server>& servers) override;
  unsigned last_index_;
};

}
}
#endif
