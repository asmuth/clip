/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <memory>
#include "fnord/net/http/httpconnection.h"
#include "fnord/net/http/httprouter.h"

namespace fnord {
namespace http {

std::unique_ptr<HTTPHandler> HTTPRouter::getHandler(
    HTTPConnection* conn,
    HTTPRequest* req) {
  HTTPHandler* handler = nullptr;
  handler = new NoSuchRouteHandler(conn, req);

  return std::unique_ptr<HTTPHandler>(handler);
}

HTTPRouter::NoSuchRouteHandler::NoSuchRouteHandler(
    HTTPConnection* conn,
    HTTPRequest* req) :
    conn_(conn),
    req_(req) {
  res_.populateFromRequest(*req);
}

void HTTPRouter::NoSuchRouteHandler::handleHTTPRequest() {
  res_.setStatus(kStatusNotFound);
  res_.addBody("Not Found");

  conn_->writeResponse(
      res_,
      std::bind(&HTTPConnection::finishResponse, conn_));
}

}
}

