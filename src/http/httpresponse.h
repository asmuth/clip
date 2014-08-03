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
#include <string>

namespace fnordmetric {
namespace http {
class HTTPOutputStream;

class HTTPResponse : public HTTPMessage {
public:
  HTTPResponse();

  void setStatus(int status);

  void writeToOutputStream(HTTPOutputStream* output);
  void populateFromRequest(const HTTPRequest& request);

protected:
  int status_;
};

}
}
#endif
