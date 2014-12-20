/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/inspect.h>
#include <fnord/net/http/httpconnection.h>
#include <fnord/net/http/httpservice.h>

namespace fnord {
namespace http {

HTTPServiceHandler::HTTPServiceHandler(
    HTTPService* service,
    HTTPConnection* conn,
    HTTPRequest* req) :
    service_(service),
    conn_(conn),
    req_(req) {
  res_.populateFromRequest(*req);
}

void HTTPServiceHandler::handleHTTPRequest() {
  conn_->readRequestBody([this] (
      const void* data,
      size_t size,
      bool last_chunk) {
    iputs("read body chunk: $0", size);

    if (last_chunk) {
      dispatchRequest();
    }
  });
}

void HTTPServiceHandler::dispatchRequest() {
  res_.setStatus(http::kStatusOK);
  res_.addBody("fnord");
  conn_->writeResponse(res_, std::bind(&HTTPConnection::finishResponse, conn_));
}

}
}

