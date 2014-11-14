/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPRESPONSE_H
#define _FNORDMETRIC_WEB_HTTPRESPONSE_H
#include <fnordmetric/http/httpmessage.h>
#include <fnordmetric/http/httprequest.h>
#include <fnordmetric/http/status.h>
#include <string>

namespace fnord {
namespace http {
class HTTPOutputStream;

class HTTPResponse : public HTTPMessage {
public:
  HTTPResponse();

  void setStatus(int status_code, const std::string& status);
  void setStatus(const HTTPStatus& status);

  void writeToOutputStream(HTTPOutputStream* output);
  void populateFromRequest(const HTTPRequest& request);

  int statusCode() const { return status_code_; }

protected:
  int status_code_;
  std::string status_;
};

}
}
#endif
