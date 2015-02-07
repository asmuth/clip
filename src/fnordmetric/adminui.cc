/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/uri.h>
#include <fnordmetric/environment.h>
#include <fnordmetric/adminui.h>

namespace fnordmetric {

AdminUI::AdminUI(
    std::string path_prefix /* = "/admin" */) :
    path_prefix_(path_prefix),
    webui_bundle_("FnordMetric"),
    webui_mount_(&webui_bundle_, path_prefix) {
  webui_bundle_.addComponent("fnord/3rdparty/codemirror.js");
  webui_bundle_.addComponent("fnord/3rdparty/fontawesome.woff");
  webui_bundle_.addComponent("fnord/3rdparty/fontawesome.css");
  webui_bundle_.addComponent("fnord/3rdparty/reset.css");
  webui_bundle_.addComponent("fnord/components/fn-table.css");
  webui_bundle_.addComponent("fnord/components/fn-button.css");
  webui_bundle_.addComponent("fnord/components/fn-modal.css");
  webui_bundle_.addComponent("fnord/components/fn-tabbar.css");
  webui_bundle_.addComponent("fnord/components/fn-message.css");
  webui_bundle_.addComponent("fnord/components/fn-tooltip.css");
  webui_bundle_.addComponent("fnord/themes/midnight-blue.css");

  webui_bundle_.addComponent("fnord/fnord.js");
  webui_bundle_.addComponent("fnord/components/fn-appbar.html");
  webui_bundle_.addComponent("fnord/components/fn-button.html");
  webui_bundle_.addComponent("fnord/components/fn-button-group.html");
  webui_bundle_.addComponent("fnord/components/fn-icon.html");
  webui_bundle_.addComponent("fnord/components/fn-input.html");
  webui_bundle_.addComponent("fnord/components/fn-loader.html");
  webui_bundle_.addComponent("fnord/components/fn-menu.html");
  webui_bundle_.addComponent("fnord/components/fn-search.html");
  webui_bundle_.addComponent("fnord/components/fn-table.html");
  webui_bundle_.addComponent("fnord/components/fn-splitpane.html");
  webui_bundle_.addComponent("fnord/components/fn-codeeditor.html");
  webui_bundle_.addComponent("fnord/components/fn-dropdown.html");
  webui_bundle_.addComponent("fnord/components/fn-datepicker.html");
  webui_bundle_.addComponent("fnord/components/fn-timeinput.html");
  webui_bundle_.addComponent("fnord/components/fn-daterangepicker.html");
  webui_bundle_.addComponent("fnord/components/fn-tabbar.html");
  webui_bundle_.addComponent("fnord/components/fn-modal.html");
  webui_bundle_.addComponent("fnord/components/fn-pager.html");
  webui_bundle_.addComponent("fnord/components/fn-tooltip.html");
  webui_bundle_.addComponent("fnord/components/fn-flexbox.html");
  webui_bundle_.addComponent("fnord/components/fn-tooltip.html");
  webui_bundle_.addComponent("fnord-metricdb/metric-explorer-list.html");
  webui_bundle_.addComponent("fnord-metricdb/metric-explorer-preview.html");
  webui_bundle_.addComponent("fnord-metricdb/metric-explorer-search.html");
  webui_bundle_.addComponent("fnord-metricdb/fn-metric-explorer.css");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-app.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-console.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-search.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-query-editor.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui.html");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui.css");
  webui_bundle_.addComponent("fnordmetric/fnordmetric-webui-util.js");
  webui_bundle_.addComponent(
    "fnordmetric/fnordmetric-embed-query-popup.html");
}

void AdminUI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  fnord::URI uri(request->uri());
  auto path = uri.path();

  if (path == "/") {
    response->setStatus(http::kStatusFound);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addHeader("Location", path_prefix_);
    return;
  }

  if (path == "/fontawesome.woff") {
    request->setURI(
        StringUtil::format(
            "$0/__components__/fnord/3rdparty/fontawesome.woff",
            path_prefix_));
  }

  webui_mount_.handleHTTPRequest(request, response);
}


}
