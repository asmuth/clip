/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEBINTERFACE_H
#define _FNORDMETRIC_WEBINTERFACE_H
#include <memory>
#include <fnord-http/httpservice.h>
#include <fnord-http/httprequest.h>
#include <fnord-http/httpresponse.h>
#include <fnord-webcomponents/bundle.h>
#include <fnord-webcomponents/httpmount.h>

using namespace fnord;
namespace fnordmetric {

class AdminUI : public fnord::http::HTTPService {
public:

  AdminUI(String path_prefix = "/admin");

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

private:
  std::string path_prefix_;
  fnord::webui::Bundle webui_bundle_;
  fnord::webui::HTTPMount webui_mount_;
};

}
#endif
