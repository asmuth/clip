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

class StatsdEmitter {
public:

  StatsdEmitter();
  StatsdEmitter(const StatsdEmitter& o) = delete;
  StatsdEmitter& operator=(const StatsdEmitter& o) = delete;

  ReturnCode connect(const std::string& addr);

  void enqueueSample(
      const std::string& metric,
      const std::string& value,
      const std::map<std::string, std::string>& labels = {});

  ReturnCode emitSamples();

protected:
  int fd_;
  sockaddr_in remote_addr_;
  std::string buf_;
};

bool parseStatsdSample(
    const char** begin,
    const char* end,
    std::string* metric_id,
    std::string* series_id,
    std::string* value);

} // namespace statsd
} // namespace fnordmetric

