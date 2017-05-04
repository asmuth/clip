/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/ingest.h>
#include <metrictools/fetch_http.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>
#include <libtransport/uri/uri.h>
#include <iostream>
#include <libtransport/http/v1/http_client.h>

namespace fnordmetric {

HTTPPullIngestionTaskConfig::HTTPPullIngestionTaskConfig() :
    interval(10 * kMicrosPerSecond),
    format(IngestionSampleFormat::STATSD) {}

ReturnCode HTTPPullIngestionTask::start(
    Backend* storage_backend,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task) {
  auto c = dynamic_cast<const HTTPPullIngestionTaskConfig*>(config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->url.empty()) {
    return ReturnCode::error("ERUNTIME", "missing url");
  }

  task->reset(
      new HTTPPullIngestionTask(
          storage_backend,
          c->interval,
          c->url,
          c->format));

  return ReturnCode::success();
}

HTTPPullIngestionTask::HTTPPullIngestionTask(
    Backend* storage_backend,
    uint64_t interval,
    const std::string& url,
    IngestionSampleFormat format) :
    PeriodicIngestionTask(interval),
    storage_backend_(storage_backend),
    url_(url),
    format_(format) {}

ReturnCode HTTPPullIngestionTask::invoke() {
  logDebug("Fetching samples via HTTP from $0", url_);

  URI url(url_);
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

  return ReturnCode::success();
  //Backend::BatchInsertOptions insert_opts;
  //insert_opts.format = format_;
  ////insert_opts.metric_id_rewrite_enabled = config_->metric_id_rewrite_enabled;
  ////insert_opts.metric_id_rewrite_regex = config_->metric_id_rewrite_regex;
  ////insert_opts.metric_id_rewrite_replace = config_->metric_id_rewrite_replace;

  //const auto& body = response.body();
  //return storage_backend_->insertSamplesBatch(
  //    body.data(),
  //    body.size(),
  //    &insert_opts);
}

} // namespace fnordmetric

