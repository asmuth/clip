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
#include <metrictools/ingest.h>

namespace fnordmetric {

struct HTTPPullIngestionTaskConfig : public IngestionTaskConfig {
  HTTPPullIngestionTaskConfig();
  std::string url;
  uint64_t interval;
  IngestionSampleFormat format;
};

class HTTPPullIngestionTask : public PeriodicIngestionTask {
public:

  static ReturnCode start(
      AggregationService* aggregation_service,
      const IngestionTaskConfig* config,
      std::unique_ptr<IngestionTask>* task);

  HTTPPullIngestionTask(
      AggregationService* aggregation_service,
      uint64_t interval,
      const std::string& url,
      IngestionSampleFormat format);

  ReturnCode invoke() override;

protected:
  AggregationService* aggr_service_;
  std::string url_;
  IngestionSampleFormat format_;
};

} // namespace fnordmetric

