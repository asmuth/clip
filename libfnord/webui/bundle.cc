/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/webui/bundle.h"

namespace fnord {
namespace webui {

void Bundle::build() {
  app_html_ = "<h1>Hello World</h1>";
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

