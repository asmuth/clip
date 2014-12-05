/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_WEBUI_HTTPMOUNT_H
#define _FNORD_WEBUI_HTTPMOUNT_H
#include <fnord/http/httphandler.h>
#include <fnord/http/httprequest.h>
#include <fnord/http/httpresponse.h>

namespace fnord {
namespace webui {
class Bundle;

class HTTPMount : public http::HTTPHandler {
public:

  HTTPMount(
      Bundle* bundle,
      const std::string base_url = "/");

  bool handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

protected:
  Bundle* bundle_;
  std::string app_url_;
  std::string app_css_url_;
  std::string app_js_url_;
  std::string app_components_base_url_;
};

} // namespace webui
} // namespace fnord

#endif
