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
    util::LogEntry log_entry;
    log_entry.append("__severity__", "DEBUG");
    log_entry.printf(
        "__message__",
        "HTTP request: %s %s",
        request->unparsedMethod().c_str(),
        request->path().c_str());

    env()->logger()->log(log_entry);
  }

  auto url = request->path();
  auto done = std::bind(&xzero::HttpResponse::completed, response);

  if (url == "/") {
    response->setStatus(xzero::HttpStatus::Found);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    response->completed();
    return true;
  }

  if (url == "/admin") {
    response->setStatus(xzero::HttpStatus::Ok);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->output()->write(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.html"),
        done);
    return true;
  }

  if (url == "/favicon.ico") {
    response->setStatus(xzero::HttpStatus::Ok);
    response->addHeader("Content-Type", "image/x-icon");
    response->output()->write(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-favicon.ico"),
        done);
    return true;
  }

  if (url == "/s/fnordmetric.js") {
    response->setStatus(xzero::HttpStatus::Ok);
    response->addHeader("Content-Type", "text/javascript");
    response->output()->write(
        util::Assets::getAsset("fnordmetric-js/fnordmetric.js"),
        done);
    return true;
  }

  if (url == "/s/fnordmetric-webui.css") {
    response->setStatus(xzero::HttpStatus::Ok);
    response->addHeader("Content-Type", "text/css");
    response->output()->write(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.css"),
        done);
    return true;
  }

  if (url == "/s/fnordmetric-webui.js") {
    response->setStatus(xzero::HttpStatus::Ok);
    response->addHeader("Content-Type", "text/javascript");
    response->output()->write(
        util::Assets::getAsset("fnordmetric-webui/fnordmetric-webui.js"),
        done);
    return true;
  }

  return false;
}

}
}
