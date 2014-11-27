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
#include <fnord/base/uri.h>

namespace fnordmetric {
namespace metricdb {

std::unique_ptr<http::HTTPHandler> AdminUI::getHandler() {
  return std::unique_ptr<http::HTTPHandler>(new AdminUI());
}

AdminUI::AdminUI() :
    webui_bundle_("FnordMetric"),
    webui_mount_(&webui_bundle_, "/admin") {
  webui_bundle_.addComponent("fnord/3rdparty/codemirror.js");
  webui_bundle_.addComponent("fnord/fnord.js");
  webui_bundle_.addComponent("fnord/themes/midnight-blue.css");
  webui_bundle_.addComponent("fnord/components/fn-appbar.html");
  webui_bundle_.addComponent("fnord/components/fn-button.html");
  webui_bundle_.addComponent("fnord/components/fn-icon.html");
  webui_bundle_.addComponent("fnord/components/fn-loader.html");
  webui_bundle_.addComponent("fnord/components/fn-search.html");
  webui_bundle_.addComponent("fnord/components/fn-table.html");
  webui_bundle_.addComponent("fnord/components/fn-splitpane.html");
  webui_bundle_.addComponent("fnord/components/fn-codeeditor.html");
  webui_bundle_.addComponent("fnord/components/fn-controls.html");
  webui_bundle_.addComponent("fnord/components/fn-dropdown.html");
  webui_bundle_.addComponent("fnord/3rdparty/fontawesome.woff");
  webui_bundle_.addComponent("fnord/3rdparty/fontawesome.css");

  webui_bundle_.addComponent("fnordmetric/fnordmetric-app.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-metric-list.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-search.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-query-editor.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-metric-preview.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui.css");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui-util.js");
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

  if (webui_mount_.handleHTTPRequest(request, response)) {
    return true;
  }

  fnord::URI uri(request->getUrl());
  auto path = uri.path();

  if (path == "/") {
    response->setStatus(http::kStatusFound);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", "/admin");
    return true;
  }

  return false;
}


}
}
