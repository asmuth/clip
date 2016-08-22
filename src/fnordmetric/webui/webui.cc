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

  //if (path == "/favicon.ico") {
  //  sendAsset(
  //      response,
  //      "fnordmetric-webui/fnordmetric-favicon.ico",
  //      "image/x-icon");
  //  return true;
  //}

  //if (path == "/assets/fnordmetric.js") {
  //  sendAsset(
  //      response,
  //      "fnordmetric-js/fnordmetric.js",
  //      "text/javascript");
  //  return true;
  //}

  //if (path == "/assets/fnordmetric.css") {
  //  sendAsset(
  //      response,
  //      "fnordmetric-webui/fnordmetric-webui.css",
  //      "text/css");
  //  return true;
  //}

  //if (path == "/s/fnordmetric-webui.js") {
  //  sendAsset(
  //      response,
  //      "fnordmetric-webui/fnordmetric-webui.js",
  //      "text/javascript");
  //  return true;
  //}

  //if (path == "/s/fontawesome.woff") {
  //  sendAsset(
  //      response,
  //      "fnordmetric-webui/fontawesome.woff",
  //      "application/x-font-woff");
  //  return true;
  //}

  response->setStatus(http::kStatusNotFound);
  response->addHeader("Content-Type", "text/plain; charset=utf-8");
  response->addBody("not found");
}

//void AdminUI::sendAsset(
//    http::HTTPResponse* response,
//    const std::string& asset_path,
//    const std::string& content_type) const {
//  response->setStatus(http::kStatusOK);
//  response->addHeader("Content-Type", content_type);
//  response->addBody(util::Assets::getAsset(asset_path));
//}

std::string WebUI::getPreludeHTML() const {
  return getAssetFile("prelude.html");
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
