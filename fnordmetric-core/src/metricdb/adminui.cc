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
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>

namespace fnordmetric {
namespace metricdb {

xzero::HttpService::Handler* AdminUI::get() {
  static AdminUI self;
  return &self;
}

bool AdminUI::handleRequest(xzero::HttpRequest* request,
                            xzero::HttpResponse* response) {

  if (env()->verbose()) {
    fnord::util::LogEntry log_entry;
    log_entry.append("__severity__", "DEBUG");
    log_entry.printf(
        "__message__",
        "HTTP request: %s %s",
        request->unparsedMethod().c_str(),
        request->path().c_str());

    env()->logger()->log(log_entry);
  }

  auto url = request->path();

  if (url == "/") {
    response->setStatus(xzero::HttpStatus::Found);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    response->completed();
    return true;
  }

  if (url == "/admin") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.html",
        "text/html; charset=utf-8");
    return true;
  }

  if (url == "/favicon.ico") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-favicon.ico",
        "image/x-icon");
    return true;
  }

  if (url == "/s/fnordmetric.js") {
    sendAsset(
        response,
        "fnordmetric-js/fnordmetric.js",
        "text/javascript");
    return true;
  }

  if (url == "/s/fnordmetric-webui.css") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.css",
        "text/css");
    return true;
  }

  if (url == "/s/fnordmetric-webui.js") {
    sendAsset(
        response,
        "fnordmetric-webui/fnordmetric-webui.js",
        "text/javascript");
    return true;
  }

  return false;
}

void AdminUI::sendAsset(
    xzero::HttpResponse* response,
    const std::string& asset_path,
    const std::string& content_type) const {
  auto asset_data = util::Assets::getAsset(asset_path);
  response->setStatus(xzero::HttpStatus::Ok);
  response->addHeader("Content-Type", content_type);
  response->setContentLength(asset_data.size());

  response->output()->write(
      asset_data,
      std::bind(&xzero::HttpResponse::completed, response));
}


}
}
