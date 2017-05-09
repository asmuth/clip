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
#include <metrictools/plot.h>

namespace fnordmetric {

namespace json = libtransport::json;

HTTPServer::HTTPServer(
    const ConfigList* config,
    Backend* storage_backend) :
    config_(config),
    storage_backend_(storage_backend) {
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

  return ReturnCode::success();
}

void HTTPServer::start() {
  thread_ = std::thread(
      std::bind(&libtransport::http::HTTPServer::run, &http_server_));
}

void HTTPServer::shutdown() {
  thread_.join();
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

  if (path == "/api/v1/plot") {
    handleRequest_PLOT(request, response);
    return;
  }

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

void HTTPServer::handleRequest_PLOT(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  auto params = URI(request->uri()).queryParams();

  PlotBuilder plot_builder(config_, storage_backend_);
  for (const auto& p : params) {
    auto rc = plot_builder.addArgument(p.first, p.second);
    if (!rc.isSuccess()) {
      response->setStatus(http::kStatusBadRequest);
      response->addHeader("Content-Type", "text/plain; charset=utf-8");
      response->addBody(rc.getMessage());
      return;
    }
  }

  Plot plot;
  auto rc = plot_builder.getPlot(&plot);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusBadRequest);
    response->addHeader("Content-Type", "text/plain; charset=utf-8");
    response->addBody(rc.getMessage());
    return;
  }

  std::string plot_target;
  rc = renderPlot(&plot, &plot_target);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusBadRequest);
    response->addHeader("Content-Type", "text/plain; charset=utf-8");
    response->addBody(rc.getMessage());
    return;
  }

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "text/html; charset=utf-8");
  response->addBody(plot_target);
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

ListenHTTPTaskConfig::ListenHTTPTaskConfig() :
    bind("0.0.0.0"),
    port(8080) {}

ReturnCode startHTTPListener(
    Backend* storage_backend,
    const ConfigList* config,
    const ListenerConfig* task_config,
    std::unique_ptr<Task>* task) {
  auto c = dynamic_cast<const ListenHTTPTaskConfig*>(task_config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->port == 0) {
    return ReturnCode::error("ERUNTIME", "missing port");
  }

  std::unique_ptr<HTTPServer> http_server(new HTTPServer(config, storage_backend));
  auto rc = http_server->listenAndRun(c->bind, c->port);
  if (!rc.isSuccess()) {
    return rc;
  }

  *task = std::move(http_server);
  return ReturnCode::success();
}

} // namespace fnordmetric

