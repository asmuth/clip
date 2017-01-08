/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/webui/webui.h>
#include <metricd/util/logging.h>
#include <metricd/util/fileutil.h>
#include <libtransport/json/json.h>
#include <libtransport/json/json_object.h>
#include <libtransport/json/json_writer.h>

namespace fnordmetric {

namespace json = libtransport::json;

WebUI::WebUI(
    const std::string& dynamic_asset_path /* = "" */) :
    dynamic_asset_path_(dynamic_asset_path) {}

void WebUI::handleHTTPRequest(
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

  if (StringUtil::beginsWith(path, "/ui/")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(getPreludeHTML());
    return;
  }

  if (StringUtil::beginsWith(path, "/embed/chart")) {
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

  if (StringUtil::beginsWith(path, "/embed/top_series")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    std::string body = getAssetFile("embed/top_series/top_series.html");
    std::string js_src;
    std::string css_src;
    js_src += getAssetFile("embed/top_series/top_series.js");
    js_src += getAssetFile("embed/top_series/top_series_table.js");
    js_src += getAssetFile("embed/top_series/table_event_handler.js");
    js_src += getAssetFile("embed/top_series/top_series_sparkline_plot.js");
    js_src += getAssetFile("embed/top_series/sparkline_linear_domain.js");
    js_src += getAssetFile("embed/svg_helper.js");
    js_src += getAssetFile("util/http.js");
    js_src += getAssetFile("util/dom.js");
    css_src += getAssetFile("embed/top_series/top_series.css");

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

std::string WebUI::getPreludeHTML() const {
  return getAssetFile("prelude.html");
}

std::string WebUI::getAppHTML() const {
  auto assets_lst = FileUtil::read(
      FileUtil::joinPaths(dynamic_asset_path_, "assets.lst")).toString();

  std::string app_html;
  for (const auto& f : StringUtil::split(assets_lst, "\n")) {
    auto file_path = FileUtil::joinPaths(dynamic_asset_path_, f);
    if (!FileUtil::exists(file_path)) {
      continue;
    }

    if (StringUtil::endsWith(f, ".html")) {
      app_html += FileUtil::read(file_path).toString();
    }

    if (StringUtil::endsWith(f, ".js")) {
      auto content = FileUtil::read(file_path).toString();
      app_html += "<script>" + content + "</script>";
    }

    if (StringUtil::endsWith(f, ".css")) {
      auto content = FileUtil::read(file_path).toString();
      app_html += "<style type='text/css'>" + content + "</style>";
    }
  }

  return app_html;
}

std::string WebUI::getAssetFile(const std::string& file) const {
  if (!dynamic_asset_path_.empty()) {
    auto file_path = FileUtil::joinPaths(dynamic_asset_path_, file);
    if (FileUtil::exists(file_path)) {
      return FileUtil::read(file_path).toString();
    }
  }

  return "";
}

}
