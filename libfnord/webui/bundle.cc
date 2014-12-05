/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/assets.h"
#include "fnord/base/exception.h"
#include "fnord/base/stringutil.h"
#include "fnord/io/fileutil.h"
#include "fnord/webui/bundle.h"

namespace fnord {
namespace webui {

static const char kBaseHTML[] = R"html(<!DOCTYPE html>
<html>
  <head>
    <title>{{title}}</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-type" />
    <link href="{{app_css_path}}" rel="stylesheet" type="text/css" />
    <link href="/favicon.ico" rel="icon" type="image/x-icon" />
    <style type="text/css">
      body[data-unresolved] * { display: none; }
    </style>
  </head>
  <body data-unresolved>
    {{body}}
    <script src="{{app_js_path}}" type="text/javascript"></script>
  </body>
</html>
)html";

static const char kFnordJSReadyTrigger[] = "Fnord.ready();";

Bundle::Bundle(
    const std::string& title /* = "Fnord WebUI" */) :
    title_(title) {}

void Bundle::build(const std::string& base_path /* = "/" */) {
  std::lock_guard<std::mutex> lock_holder(mutex_);

  app_html_ = kBaseHTML;
  app_css_.clear();
  app_js_.clear();

  StringUtil::replaceAll(&app_html_, "{{title}}", title_);
  StringUtil::replaceAll(&app_html_, "{{app_css_path}}",
      io::FileUtil::joinPaths(base_path, "application.css"));
  StringUtil::replaceAll(&app_html_, "{{app_js_path}}",
      io::FileUtil::joinPaths(base_path, "application.js"));

  auto body_cur = app_html_.find("{{body}}");
  if (body_cur == std::string::npos) {
    RAISE(kRuntimeError, "invalid HTML base template, {{body}} missing");
  } else {
    app_html_.erase(body_cur, 8);
  }

  for (const auto& component : components_) {
    if (StringUtil::endsWith(component, ".html")) {
      const auto& asset = Assets::getAsset(component);
      app_html_.insert(body_cur, asset);
      body_cur += asset.size();
    }

    if (StringUtil::endsWith(component, ".js")) {
      app_js_.append(Assets::getAsset(component));
    }

    if (StringUtil::endsWith(component, ".css")) {
      app_css_.append(Assets::getAsset(component));
    }
  }

  StringUtil::replaceAll(&app_js_, kFnordJSReadyTrigger, "");
  app_js_.append(kFnordJSReadyTrigger);
}

void Bundle::addComponent(const std::string& component_path) {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  components_.emplace_back(component_path);
}

bool Bundle::hasComponent(const std::string& component_path) const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return std::find(
      components_.begin(),
      components_.end(),
      component_path) != components_.end();
}

const std::string& Bundle::applicationHTML() const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return app_html_;
}

const std::string& Bundle::applicationCSS() const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return app_css_;
}

const std::string& Bundle::applicationJS() const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return app_js_;
}

} // namespace webui
} // namespace fnord

