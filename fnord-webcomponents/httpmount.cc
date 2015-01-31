/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/assets.h>
#include <fnord-base/uri.h>
#include <fnord-base/stringutil.h>
#include <fnord-base/io/fileutil.h>
#include <fnord/webui/bundle.h>
#include <fnord/webui/httpmount.h>

namespace fnord {
namespace webui {

HTTPMount::HTTPMount(
    fnord::webui::Bundle* bundle,
    const std::string base_url /* = "/" */) :
    bundle_(bundle),
    app_url_(base_url),
    app_css_url_(FileUtil::joinPaths(base_url, "application.css")),
    app_js_url_(FileUtil::joinPaths(base_url, "application.js")),
    app_components_base_url_(
        FileUtil::joinPaths(base_url, "__components__") + "/") {}

bool HTTPMount::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  fnord::URI uri(request->getUrl());
  auto path = uri.path();

  bundle_->build(app_url_); // FIXPAUL

  if (path == app_url_) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    response->addBody(bundle_->applicationHTML());
    return true;
  }

  if (path == app_js_url_) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "application/javascript");
    response->addBody(bundle_->applicationJS());
    return true;
  }

  if (path == app_css_url_) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "text/css");
    response->addBody(bundle_->applicationCSS());
    return true;
  }

  if (StringUtil::beginsWith(path, app_components_base_url_)) {
    auto component_path = path.substr(app_components_base_url_.length());

    if (bundle_->hasComponent(component_path)) {
      response->setStatus(http::kStatusOK);
      response->addHeader("Content-Type", "text/plain"); // FIXPAUL
      response->addBody(fnord::Assets::getAsset(component_path));
      return true;
    }
  }

  return false;
}

} // namespace webui
} // namespace fnord

