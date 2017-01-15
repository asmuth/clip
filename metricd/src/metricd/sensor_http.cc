/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/metric_service.h>
#include <metricd/sensor_http.h>
#include <metricd/util/time.h>
#include <libtransport/uri/uri.h>
#include <iostream>
#include <libtransport/http/v1/http_client.h>

namespace fnordmetric {

HTTPSensorTask::HTTPSensorTask(
    const HTTPSensorConfig* config,
    MetricService* metric_service) :
    config_(config),
    metric_service_(metric_service),
    next_invocation_(0) {}

uint64_t HTTPSensorTask::getNextInvocationTime() const {
  return next_invocation_;
}

ReturnCode HTTPSensorTask::invoke() {
  next_invocation_ = MonotonicClock::now() + 1 * kMicrosPerSecond;

  URI url(config_->http_url);
  libtransport::http::HTTPClient client;
  if (!client.connect(url.host(), url.port())) {
    return ReturnCode::error("EIO", client.getError());
  }

  auto request = libtransport::http::HTTPRequest::mkGet(url.pathAndQuery());
  libtransport::http::HTTPResponse response;
  if (!client.executeRequest(request, &response)) {
    return ReturnCode::error("EIO", client.getError());
  }

  if (response.statusCode() < 200 && response.statusCode() >= 300) {
    return ReturnCode::errorf(
        "EIO",
        "invalid HTTP response code: $0",
        response.statusCode());
  }

  MetricService::BatchInsertOptions insert_opts;
  insert_opts.metric_id_rewrite_enabled = config_->metric_id_rewrite_enabled;
  insert_opts.metric_id_rewrite_regex = config_->metric_id_rewrite_regex;
  insert_opts.metric_id_rewrite_replace = config_->metric_id_rewrite_replace;
  insert_opts.series_id_rewrite_enabled = config_->series_id_rewrite_enabled;
  insert_opts.series_id_rewrite_regex = config_->series_id_rewrite_regex;
  insert_opts.series_id_rewrite_replace = config_->series_id_rewrite_replace;

  const auto& body = response.body();
  return metric_service_->insertSamplesBatch(
      body.data(),
      body.size(),
      &insert_opts);
}

} // namespace fnordmetric

