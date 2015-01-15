/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_WEBUI_VIEW_H
#define _FNORD_WEBUI_VIEW_H
#include <string>
#include <vector>
#include <mutex>

namespace fnord {
namespace webui {

class WebView {
public:

  virtual void render(WebRequest* request) = 0;

};

} // namespace webui
} // namespace fnord

#endif
