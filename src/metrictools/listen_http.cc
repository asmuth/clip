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
#include <metrictools/listen_http.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/stringutil.h>
#include <metrictools/util/fileutil.h>
#include <metrictools/storage/backend.h>
#include <libtransport/json/json.h>
#include <libtransport/json/json_object.h>
#include <libtransport/json/json_writer.h>

namespace fnordmetric {

namespace json = libtransport::json;

HTTPServer::HTTPServer(Backend* backend) : backend_(backend) {
  http_server_.setRequestHandler(
      std::bind(
          &HTTPServer::handleRequest,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
}

ReturnCode HTTPServer::listenAndRun(const std::string& addr, int port) {
  logInfo("Starting HTTP server on $0:$1", addr, port);

  if (!http_server_.listen(addr, port)) {
    return ReturnCode::error("ERUNTIME", "listen() failed");
  }

  http_server_.run();
  return ReturnCode::success();
}

void HTTPServer::handleRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  logDebug(
      "HTTP request: $0 $1",
      http::getHTTPMethodName(request->method()),
      request->uri());

  URI uri(request->uri());
  const auto& path = uri.path();

  if (path == "/") {
    response->setStatus(http::kStatusOK);
    response->addBody("ok");
    return;
  }

  //if (path == "/api/v1/query") {
  //  handleRequest_QUERY(request, response);
  //  return;
  //}

  if (path == "/favicon.ico") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "image/x-icon");
    response->addBody(getAssetFile("favicon.ico"));
    return;
  }

  response->setStatus(http::kStatusNotFound);
  response->addHeader("Content-Type", "text/plain; charset=utf-8");
  response->addBody("not found");
}

std::string HTTPServer::getPreludeHTML() const {
  return getAssetFile("prelude.html");
}

std::string HTTPServer::getAppHTML() const {
  auto assets_lst = getAssetFile("assets.lst");

  std::string app_html;
  for (const auto& f : StringUtil::split(assets_lst, "\n")) {
    if (StringUtil::endsWith(f, ".html")) {
      app_html += getAssetFile(f);
    }

    if (StringUtil::endsWith(f, ".js")) {
      auto content = getAssetFile(f);
      app_html += "<script>" + content + "</script>";
    }

    if (StringUtil::endsWith(f, ".css")) {
      auto content = getAssetFile(f);
      app_html += "<style type='text/css'>" + content + "</style>";
    }
  }

  return app_html;
}

std::string HTTPServer::getAssetFile(const std::string& file) const {
  if (!dynamic_asset_path_.empty()) {
    auto file_path = FileUtil::joinPaths(dynamic_asset_path_, file);
    if (FileUtil::exists(file_path)) {
      return FileUtil::read(file_path).toString();
    }
  }

  return "";
}

void HTTPServer::setAssetPath(const std::string& path) {
  dynamic_asset_path_ = path;
}


HTTPPushIngestionTaskConfig::HTTPPushIngestionTaskConfig() :
    bind("0.0.0.0"),
    port(8080) {}

ReturnCode HTTPPushIngestionTask::start(
    Backend* storage_backend,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task) {
  auto c = dynamic_cast<const HTTPPushIngestionTaskConfig*>(config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->port == 0) {
    return ReturnCode::error("ERUNTIME", "missing port");
  }

  auto self = new HTTPPushIngestionTask(storage_backend);
  task->reset(self);

  auto rc = self->listen(c->bind, c->port);
  if (!rc.isSuccess()) {
    return rc;
  }

  return ReturnCode::success();
}

HTTPPushIngestionTask::HTTPPushIngestionTask(
    Backend* storage_backend) :
    storage_backend_(storage_backend) {
  http_server_.setRequestHandler(
      std::bind(
          &HTTPPushIngestionTask::handleRequest,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
}

ReturnCode HTTPPushIngestionTask::listen(const std::string& addr, int port) {
  logInfo("Starting HTTP server on $0:$1", addr, port);

  if (http_server_.listen(addr, port)) {
    return ReturnCode::success();
  } else {
    return ReturnCode::error("ERUNTIME", "listen() failed");
  }
}

void HTTPPushIngestionTask::start() {
  http_server_.run();
}

void HTTPPushIngestionTask::shutdown() {}

void HTTPPushIngestionTask::handleRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  URI uri(request->uri());
  auto path = uri.path();

  static const char kMetricsUrl[] = "/metrics";
  if (path != kMetricsUrl || request->method() != http::HTTPRequest::M_POST) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("ERROR: please send metrics to POST /metrics\n");
    return;
  }

  //Backend::BatchInsertOptions opts;
  //Opts.format = IngestionMeasurementCoding::STATSD;
  //If (request->getHeader("Content-Type") == "application/json") {
  //  opts.format = IngestionMeasurementCoding::JSON;
  //}
  //If (request->getHeader("Content-Type") == "text/plain") {
  //  opts.format = IngestionMeasurementCoding::STATSD;
  //}

  //Auto rc = storage_backend_->insertMeasurementsBatch(
  //    request->body().data(),
  //    request->body().size(),
  //    &opts);

  //If (rc.isSuccess()) {
  //  response->setStatus(http::kStatusCreated);
  //  response->addBody("OK\n");
  //} else {
  //  response->setStatus(http::kStatusInternalServerError);
  //  response->addBody(StringUtil::format("ERROR: $0\n", rc.getMessage()));
  //}
}

} // namespace fnordmetric

