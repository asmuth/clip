/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEBINTERFACE_H
#define _FNORDMETRIC_WEBINTERFACE_H
#include <memory>
#include <metricd/http/httphandler.h>
#include <metricd/http/httprequest.h>
#include <metricd/http/httpresponse.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class AdminUI : public http::HTTPHandler {
public:

  static std::unique_ptr<http::HTTPHandler> getHandler();

  bool handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

private:

  void sendAsset(
      http::HTTPResponse* response,
      const std::string& asset_path,
      const std::string& content_type) const;

};

}
}
#endif
