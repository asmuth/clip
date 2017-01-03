/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_HTTPCONNECTIONHANDLER_H
#define _libstx_HTTPCONNECTIONHANDLER_H
#include <metricd/transport/http/httprequest.h>
#include <metricd/transport/http/httpresponse.h>

namespace fnordmetric {
namespace http {

class HTTPServerConnectionHandler {
public:

  virtual ~HTTPServerConnectionHandler() {}

  virtual bool handleHTTPRequest(
      HTTPRequest* request,
      HTTPResponse* response) = 0;

};

}
}
#endif
