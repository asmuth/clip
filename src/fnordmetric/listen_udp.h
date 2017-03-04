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
#include <atomic>
#include "fnordmetric/util/return_code.h"
#include "fnordmetric/sample.h"
#include "fnordmetric/aggregation_service.h"

namespace fnordmetric {

class StatsdServer : public IngestionTask {
public:

  StatsdServer(AggregationService* aggr_service);
  ~StatsdServer();

  ReturnCode listen(const std::string& addr, int port);
  ReturnCode listenAndStart(const std::string& addr, int port);

  ReturnCode start() override;
  void shutdown() override;

protected:

  void handlePacket(const char* pkt, size_t pkt_len);

  AggregationService* aggr_service_;
  int ssock_;
  std::atomic<bool> running_;
};

} // namespace fnordmetric

