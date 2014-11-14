/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/metricdb/adminui.h>
#include <fnordmetric/util/assets.h>
#include <fnordmetric/util/uri.h>

namespace fnordmetric {
namespace metricdb {

std::unique_ptr<http::HTTPHandler> AdminUI::getHandler() {
  return std::unique_ptr<http::HTTPHandler>(new AdminUI());
}

bool AdminUI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {

  if (env()->verbose()) {
    fnord::util::LogEntry log_entry;
    log_entry.append("__severity__", "DEBUG");
    log_entry.printf(
        "__message__",
        "HTTP request: %s %s",
        request->getMethod().c_str(),
        request->getUrl().c_str());

    env()->logger()->log(log_entry);
  }

  util::URI uri(request->getUrl());
  auto path = uri.path();

  if (path == "/") {
    response->setStatus(http::kStatusFound);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    return true;
  }

  if (path == "/admin") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.html",
        "text/html; charset=utf-8");
    return true;
  }

  if (path == "/favicon.ico") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-favicon.ico",
        "image/x-icon");
    return true;
  }

  if (path == "/s/fnordmetric.js") {
    sendAsset(
        response,
        "fnordmetric-js/fnordmetric.js",
        "text/javascript");
    return true;
  }

  if (path == "/s/fnordmetric-webui.css") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.css",
        "text/css");
    return true;
  }

  if (path == "/s/fnordmetric-webui.js") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.js",
        "text/javascript");
    return true;
  }

  if (path == "/s/fontawesome.woff") {
    sendAsset(
        response,
        "fnordmetric-webui/fontawesome.woff",
        "application/x-font-woff");
    return true;
  }

  return false;
}

void AdminUI::sendAsset(
    http::HTTPResponse* response,
    const std::string& asset_path,
    const std::string& content_type) const {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", content_type);
  response->addBody(util::Assets::getAsset(asset_path));
}


}
}
