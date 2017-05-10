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
    Backend* storage_backend,
    std::unique_ptr<DashboardMap> dashboard_map) :
    config_(config),
    storage_backend_(storage_backend),
    dashboard_map_(std::move(dashboard_map)) {
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
    response->setStatus(http::kStatusFound);
    response->addHeader("Location", "/admin/");
    return;
  }

  if (path == "/api/v1/plot") {
    handleAPIRequest_PLOT(request, response);
    return;
  }

  if (StringUtil::beginsWith(path, "/admin/")) {
    handleAdminUIRequest(request, response);
    return;
  }

  if (StringUtil::beginsWith(path, "/dashboard/")) {
    handleDashboardRequest(request, response);
    return;
  }

  if (StringUtil::beginsWith(path, "/assets/")) {
    handleAssetRequest(request, response);
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

void HTTPServer::handleAdminUIRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
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

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "text/plain; charset=utf-8");
  response->addBody(app_html);
}

void HTTPServer::handleDashboardRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  auto path_parts = StringUtil::split(URI(request->uri()).path(), "/");
  if (path_parts.size() < 3) {
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto dashboard_id = path_parts[2];
  path_parts.erase(path_parts.begin(), path_parts.begin() + 3);

  auto dashboard_file = StringUtil::join(path_parts, "/");
  if (dashboard_file.empty()) {
    dashboard_file = "index.html";
  }

  auto dashboard_info = dashboard_map_->findDashboard(dashboard_id);
  if (!dashboard_info) {
    response->setStatus(http::kStatusNotFound);
    response->addHeader("Content-Type", "text/plain; charset=utf-8");
    response->addBody("dashboard not found");
    return;
  }

  std::string realpath;
  try {
    realpath = FileUtil::realpath(
        FileUtil::joinPaths(dashboard_info->basepath, dashboard_file));
  } catch (...) {}

  if (!StringUtil::beginsWith(realpath, dashboard_info->basepath)) {
    realpath.clear();
  }

  if (!realpath.empty() && FileUtil::exists(realpath)) {
    sendFile(response, realpath);
  } else {
    response->setStatus(http::kStatusNotFound);
    response->addHeader("Content-Type", "text/plain; charset=utf-8");
    response->addBody(
        StringUtil::format(
            "file '$0' not found in dashboard '$1' ($2)",
            dashboard_file,
            dashboard_id,
            dashboard_info->basepath));
  }
}

void HTTPServer::handleAssetRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  auto path_parts = StringUtil::split(URI(request->uri()).path(), "/");
  if (path_parts.size() < 2) {
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  path_parts.erase(path_parts.begin(), path_parts.begin() + 2);

  auto asset_basepath = FileUtil::realpath(
      FileUtil::joinPaths(config_->getDatadir(), "webui"));

  auto asset_path = StringUtil::join(path_parts, "/");

  std::string asset_realpath;
  try {
    asset_realpath = FileUtil::realpath(
        FileUtil::joinPaths(asset_basepath, asset_path));
  } catch (...) {}

  if (!StringUtil::beginsWith(asset_realpath, asset_basepath)) {
    asset_realpath.clear();
  }

  if (!asset_realpath.empty() && FileUtil::exists(asset_realpath)) {
    sendFile(response, asset_realpath);
  } else {
    response->setStatus(http::kStatusNotFound);
    response->addHeader("Content-Type", "text/plain; charset=utf-8");
    response->addBody(
        StringUtil::format(
            "file '$0' not found",
            asset_path));
  }
}

void HTTPServer::handleAPIRequest_PLOT(
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

void HTTPServer::sendFile(
    http::HTTPResponse* response,
    const std::string& file_path) {
  auto mime_type = "application/octet-stream";

  if (StringUtil::endsWith(file_path, ".html")) mime_type = "text/html; charset=utf-8";
  if (StringUtil::endsWith(file_path, ".js")) mime_type = "application/javascript; charset=utf-8";
  if (StringUtil::endsWith(file_path, ".css")) mime_type = "text/css; charset=utf-8";

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", mime_type);
  response->addBody(FileUtil::read(file_path).toString());
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

  std::unique_ptr<DashboardMap> dashboard_map(new DashboardMap());
  {
    auto rc = dashboard_map->loadConfig(config);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  std::unique_ptr<HTTPServer> http_server(
      new HTTPServer(
          config,
          storage_backend,
          std::move(dashboard_map)));

  {
    auto rc = http_server->listenAndRun(c->bind, c->port);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  *task = std::move(http_server);
  return ReturnCode::success();
}

ReturnCode DashboardMap::loadConfig(const ConfigList* config) {
  for (const auto& path : config->getDashboardPaths()) {
    if (!FileUtil::isDirectory(path)) {
      return ReturnCode::errorf("EARG", "not a valid directory: $0", path);
    }

    auto path_full = FileUtil::realpath(path);
    auto path_base = FileUtil::basedir(path_full);
    auto path_trail = path_full.substr(path_base.size() + 1);

    if (map_.count(path_trail) > 0) {
      return ReturnCode::errorf(
          "EARG",
          "duplicate dashboard directory: $0",
          path_trail);
    }

    DashboardInfo info;
    info.basepath = path_full;
    map_.emplace(path_trail, info);
  }

  return ReturnCode::success();
}

DashboardMap::DashboardInfo* DashboardMap::findDashboard(
    const std::string& dashboard_id) {
  auto iter = map_.find(dashboard_id);
  if (iter == map_.end()) {
    return nullptr;
  } else {
    return &iter->second;
  }
}

} // namespace fnordmetric

