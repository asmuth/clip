/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/webui/webui.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/fileutil.h>
#include <metrictools/query_service.h>
#include <libtransport/json/json.h>
#include <libtransport/json/json_object.h>
#include <libtransport/json/json_writer.h>

namespace fnordmetric {

namespace json = libtransport::json;

QueryService::QueryService(Backend* backend) : backend_(backend) {
  http_server_.setRequestHandler(
      std::bind(
          &QueryService::handleRequest,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
}

ReturnCode QueryService::listenAndRun(const std::string& addr, int port) {
  logInfo("Starting HTTP server on $0:$1", addr, port);

  if (!http_server_.listen(addr, port)) {
    return ReturnCode::error("ERUNTIME", "listen() failed");
  }

  http_server_.run();
  return ReturnCode::success();
}

void QueryService::handleRequest(
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
    response->addHeader("Location", "/ui/");
    return;
  }

  if (path == "/api/v1/query") {
    handleRequest_QUERY(request, response);
    return;
  }

  if (StringUtil::beginsWith(path, "/ui/")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(getPreludeHTML());
    return;
  }

  if (StringUtil::beginsWith(path, "/embed/timeseries")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    std::string body = getAssetFile("embed/chart/chart.html");
    std::string js_src;
    std::string css_src;
    js_src += getAssetFile("embed/chart/chart.js");
    js_src += getAssetFile("embed/chart/chart_plotter_linear_domain.js");
    js_src += getAssetFile("embed/chart/chart_plot.js");
    js_src += getAssetFile("embed/chart/chart_query_manager.js");
    js_src += getAssetFile("embed/svg_helper.js");
    js_src += getAssetFile("util/http.js");
    js_src += getAssetFile("embed/units.js");
    js_src += getAssetFile("embed/colors.js");
    css_src += getAssetFile("embed/chart/chart.css");

    std::string config = "{}";
    URI::getParam(uri.queryParams(), "c", &config);
    StringUtil::replaceAll(&config, "</", "<\\/");

    StringUtil::replaceAll(&body, "{{JS_SRC}}", js_src);
    StringUtil::replaceAll(&body, "{{CSS_SRC}}", css_src);
    StringUtil::replaceAll(&body, "{{PARAMS}}", config);

    response->addBody(body);
    return;
  }

  if (StringUtil::beginsWith(path, "/assets/app.html")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(getAppHTML());
    return;
  }

  if (path == "/favicon.ico") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "image/x-icon");
    response->addBody(getAssetFile("favicon.ico"));
    return;
  }

  if (path == "/fontawesome.woff") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "application/x-font-woff");
    response->addBody(getAssetFile("fontawesome.woff"));
    return;
  }

  response->setStatus(http::kStatusNotFound);
  response->addHeader("Content-Type", "text/plain; charset=utf-8");
  response->addBody("not found");
}

void QueryService::handleRequest_QUERY(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  /* read params */
  json::JSONObjectStorage json_req;
  if (request->method() == http::HTTPMessage::M_POST) {
    const auto& body = request->body();
    if (!json::readJSON(&json_req, &body) || !json_req.hasRootObject()) {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("ERROR: invalid json");
      return;
    }
  } else {
    auto params = URI(request->uri()).queryParams();

    std::string query;
    if (URI::getParam(params, "query", &query)) {
      json_req.getRootAsObject()->setString("query", query);
    }
  }

  std::string query = json_req->getString("query");
  if (query.empty()) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("missing parameter: query");
    return;
  }

  /* execute query */
  std::vector<std::string> result_header;
  std::list<std::vector<std::string>> result_rows;
  auto rc = backend_->executeQuery(query, &result_header, &result_rows);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody(StringUtil::format("ERROR: $0", rc.getMessage()));
    return;
  }

  /* write response */
  std::string json_str;
  json::JSONWriter json(&json_str);

  json.beginObject();

  json.addString("query");
  json.addString(query);
  json.addString("result");
  json.beginObject();

  json.addString("column_names");
  json.beginArray();
  for (const auto& c : result_header) {
    json.addString(c);
  }
  json.endArray();

  json.addString("rows");
  json.beginArray();
  for (const auto& row : result_rows) {
    json.beginArray();
    for (const auto& c : row) {
      json.addString(c);
    }
    json.endArray();
  }
  json.endArray();

  json.endObject();
  json.endObject();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->addBody(json_str);
}

std::string QueryService::getPreludeHTML() const {
  return getAssetFile("prelude.html");
}

std::string QueryService::getAppHTML() const {
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

std::string QueryService::getAssetFile(const std::string& file) const {
  if (!dynamic_asset_path_.empty()) {
    auto file_path = FileUtil::joinPaths(dynamic_asset_path_, file);
    if (FileUtil::exists(file_path)) {
      return FileUtil::read(file_path).toString();
    }
  }

  return "";
}

void QueryService::setAssetPath(const std::string& path) {
  dynamic_asset_path_ = path;
}

} // namespace fnordmetric

