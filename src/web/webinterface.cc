/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/web/assets.h>
#include <fnordmetric/web/webinterface.h>

namespace fnordmetric {
namespace web {

std::unique_ptr<http::HTTPHandler> WebInterface::getHandler() {
  return std::unique_ptr<http::HTTPHandler>(new WebInterface());
}

bool WebInterface::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  auto url = request->getUrl();

  if (url == "/") {
    response->setStatus(302);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    return true;
  }

  if (url == "/admin") {
    response->setStatus(200);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(Assets::getAsset("fnordmetric-webui/fnordmetric-webui.html"));
    return true;
  }

  if (url == "/favicon.ico") {
    response->setStatus(200);
    response->addHeader("Content-Type", "image/x-icon");
    response->addBody(
        Assets::getAsset("fnordmetric-webui/fnordmetric-favicon.ico"));
    return true;
  }

  if (url == "/s/fnordmetric.js") {
    response->setStatus(200);
    response->addHeader("Content-Type", "text/javascript");
    response->addBody(Assets::getAsset("fnordmetric-js/fnordmetric.js"));
    return true;
  }

  if (url == "/s/fnordmetric-editor.js") {
    response->setStatus(200);
    response->addHeader("Content-Type", "text/javascript");
    response->addBody(Assets::getAsset("fnordmetric-js/fnordmetric-editor.js"));
    return true;
  }

  if (url == "/s/fnordmetric-webui.css") {
    response->setStatus(200);
    response->addHeader("Content-Type", "text/css");
    response->addBody(
        Assets::getAsset("fnordmetric-webui/fnordmetric-webui.css"));
    return true;
  }

  if (url == "/s/fnordmetric-webui.js") {
    response->setStatus(200);
    response->addHeader("Content-Type", "text/javascript");
    response->addBody(
        Assets::getAsset("fnordmetric-webui/fnordmetric-webui.js"));
    return true;
  }

  return false;
}

}
}
