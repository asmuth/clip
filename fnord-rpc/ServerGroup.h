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

class ServerGroup {
public:
  ServerGroup();

  std::string getServerForNextRequest();

  void addServer(const std::string& addr);
  void removeServer(const std::string& addr);
  void markServerAsDown(const std::string& addr);

protected:
  enum kServerState {
    S_UP,
    S_DOWN
  };

  struct Server {
    Server(const std::string& addr);
    std::string addr;
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

class RoundRobinServerGroup : public ServerGroup {
public:
  RoundRobinServerGroup();
protected:
  int pickServerForNextRequest(const std::vector<Server>& servers) override;
  unsigned last_index_;
};

}
}
#endif
