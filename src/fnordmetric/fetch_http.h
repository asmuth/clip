/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <fnordmetric/ingest.h>

namespace fnordmetric {

struct HTTPIngestionTaskConfig : public IngestionTaskConfig {
  std::string http_url;
};

class HTTPIngestionTask : public IngestionTask {
public:

  HTTPIngestionTask(const HTTPIngestionTaskConfig* config, MetricService* metric_service);

  uint64_t getNextInvocationTime() const override;

  ReturnCode invoke() override;

protected:
  const HTTPIngestionTaskConfig* config_;
  MetricService* metric_service_;
  uint64_t next_invocation_;
};

} // namespace fnordmetric

