/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/buffer.h>
#include <fnord/net/udpserver.h>
#include <fnord/thread/taskscheduler.h>
#include <fnordmetric/metricdb/metricrepository.h>

namespace fnordmetric {
namespace metricdb {

class StatsdServer {
public:

  StatsdServer(
      IMetricRepository* metric_repo,
      fnord::thread::TaskScheduler* server_scheduler,
      fnord::thread::TaskScheduler* work_scheduler);

  void listen(int port);

  static char const* parseStatsdSample(
      char const* begin,
      char const* end,
      std::string* key,
      std::string* value,
      std::vector<std::pair<std::string, std::string>>* labels);

protected:

  void messageReceived(const fnord::Buffer& msg);

  IMetricRepository* metric_repo_;
  fnord::net::UDPServer udp_server_;
};


}
}
