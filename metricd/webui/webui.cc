/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/webui/webui.h>
//#include <fnordmetric/util/assets.h>
#include <fnordmetric/util/uri.h>
#include <fnordmetric/util/logging.h>

namespace fnordmetric {

WebUI::WebUI(
    const String& dynamic_asset_path /* = "" */) :
    dynamic_asset_path_(dynamic_asset_path) {}

void WebUI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {

  logDebug(
      "HTTP request: $0 $1",
      request->method(),
      request->uri());

  URI uri(request->uri());
  const auto& path = uri.path();

  if (path == "/") {
    response->setStatus(http::kStatusFound);
    response->addHeader("Location", "/metrics/");
    return;
  }

  if (StringUtil::beginsWith(path, "/metrics")) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(getPreludeHTML());
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
