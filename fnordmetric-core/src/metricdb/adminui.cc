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

namespace fnordmetric {
namespace metricdb {

std::unique_ptr<http::HTTPHandler> AdminUI::getHandler() {
  return std::unique_ptr<http::HTTPHandler>(new AdminUI());
}

bool AdminUI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {

  if (env()->verbose()) {
    util::LogEntry log_entry;
    log_entry.append("__severity__", "DEBUG");
    log_entry.printf(
        "__message__",
        "HTTP request: %s %s",
        request->getMethod().c_str(),
        request->getUrl().c_str());

    env()->logger()->log(log_entry);
  }

  auto url = request->getUrl();

  if (url == "/") {
    response->setStatus(http::kStatusFound);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    return true;
  }

  if (url == "/admin") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.html"));
    return true;
  }

  if (url == "/favicon.ico") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "image/x-icon");
    response->addBody(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-favicon.ico"));
    return true;
  }

  if (url == "/s/fnordmetric.js") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/javascript");
    response->addBody(util::Assets::getAsset("fnordmetric-js/fnordmetric.js"));
    return true;
  }

  if (url == "/s/fnordmetric-webui.css") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/css");
    response->addBody(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.css"));
    return true;
  }

  if (url == "/s/fnordmetric-webui.js") {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/javascript");
    response->addBody(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.js"));
    return true;
  }

  return false;
}

}
}
