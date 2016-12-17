/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include "metricd/util/return_code.h"
#include "metricd/sample.h"

namespace fnordmetric {
class MetricService;
namespace statsd {

class StatsdServer {
public:

  StatsdServer(
      MetricService* metric_service);

  ~StatsdServer();

  ReturnCode listen(const std::string& addr, int port);
  ReturnCode listenAndStart(const std::string& addr, int port);

  ReturnCode start();
  void shutdown();

protected:

  void handlePacket(const char* pkt, size_t pkt_len);

  int ssock_;
  MetricService* metric_service_;
  std::thread thread_;
  std::atomic<bool> running_;
};

} // namespace statsd
} // namespace fnordmetric

