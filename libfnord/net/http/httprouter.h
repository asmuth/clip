/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPROUTER_H
#define _FNORDMETRIC_HTTPROUTER_H

#include "fnord/net/http/httphandler.h"
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponse.h"

namespace fnord {
namespace http {

class HTTPRouter : public HTTPHandlerFactory {
public:

  class NoSuchRouteHandler : public HTTPHandler {
  public:
    NoSuchRouteHandler(HTTPConnection* conn, HTTPRequest* req);
    void handleHTTPRequest() override;
  protected:
    HTTPConnection* conn_;
    HTTPRequest* req_;
    HTTPResponse res_;
  };

  std::unique_ptr<HTTPHandler> getHandler(
      HTTPConnection* conn,
      HTTPRequest* req) override;

};

}
}
#endif
