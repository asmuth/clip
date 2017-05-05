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
#include "metrictools/util/return_code.h"
#include "metrictools/measure.h"
#include "metrictools/collect.h"

namespace fnordmetric {

struct UDPIngestionTaskConfig : public IngestionTaskConfig {
  UDPIngestionTaskConfig();
  std::string bind;
  uint16_t port;
  MeasurementCoding format;
};

class UDPListener : public IngestionTask {
public:

  static ReturnCode start(
      Backend* storage_backend,
      const IngestionTaskConfig* config,
      std::unique_ptr<IngestionTask>* task);

  UDPListener(
      Backend* storage_backend,
      MeasurementCoding format);

  ~UDPListener();

  ReturnCode listen(const std::string& addr, int port);

  void start() override;
  void shutdown() override;

protected:
  Backend* storage_backend_;
  MeasurementCoding format_;
  int ssock_;
  std::atomic<bool> running_;
};

} // namespace fnordmetric

