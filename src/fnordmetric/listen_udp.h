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
#pragma once
#include <stdlib.h>
#include <atomic>
#include "fnordmetric/util/return_code.h"
#include "fnordmetric/sample.h"
#include "fnordmetric/aggregation_service.h"

namespace fnordmetric {

struct UDPIngestionTaskConfig : public IngestionTaskConfig {
  UDPIngestionTaskConfig();
  std::string bind;
  uint16_t port;
  IngestionSampleFormat format;
};

class UDPListener : public IngestionTask {
public:

  static ReturnCode start(
      AggregationService* aggregation_service,
      const IngestionTaskConfig* config,
      std::unique_ptr<IngestionTask>* task);

  UDPListener(
      AggregationService* aggr_service,
      IngestionSampleFormat format);

  ~UDPListener();

  ReturnCode listen(const std::string& addr, int port);
  ReturnCode start() override;

  void shutdown() override;

protected:
  AggregationService* aggr_service_;
  IngestionSampleFormat format_;
  int ssock_;
  std::atomic<bool> running_;
};

} // namespace fnordmetric

