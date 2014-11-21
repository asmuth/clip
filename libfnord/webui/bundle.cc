/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/base/stringutil.h"
#include "fnord/webui/bundle.h"

namespace fnord {
namespace webui {

static const char kBaseHTML[] = R"(<!DOCTYPE html>
<html>
  <head>
    <title>{{title}}</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-type" />
    <link href="{{basepath}}application.css" rel="stylesheet" type="text/css" />
    <link href="/favicon.ico" rel="icon" type="image/x-icon" />
  </head>
  <body>
    {{body}}
    <script src="{{basepath}}application.js" type="text/javascript"></script>
  </body>
</html>
)";

Bundle::Bundle(
    const std::string& title /* = "Fnord WebUI" */) :
    title_(title) {}

void Bundle::build(const std::string& base_path /* = "/" */) {
  app_html_ = kBaseHTML;
  StringUtil::replaceAll(&app_html_, "{{basepath}}", base_path);
  StringUtil::replaceAll(&app_html_, "{{title}}", title_);

  auto body_cur = app_html_.find("{{body}}");
  if (body_cur == std::string::npos) {
    RAISE(kRuntimeError, "invalid HTML base template, {{body}} missing");
  } else {
    app_html_.erase(body_cur, 8);
  }

  app_html_.insert(body_cur, "fnord");
  body_cur += 5;
  app_html_.insert(body_cur, "bar");
}

const std::string& Bundle::applicationHTML() {
  return app_html_;
}

const std::string& Bundle::applicationCSS() {
  return app_css_;
}

const std::string& Bundle::applicationJS() {
  return app_js_;
}

} // namespace webui
} // namespace fnord

