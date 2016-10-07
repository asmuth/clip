/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <stdlib.h>
#include <memory>
#include "fnordmetric/transport/http/httpserverconnection.h"
#include "fnordmetric/transport/http/httprouter.h"
#include "fnordmetric/transport/http/httpservice.h"

namespace fnordmetric {
namespace http {

void HTTPRouter::addRoute(
    std::function<bool (HTTPRequest*)> predicate,
    StreamingHTTPService* service) {
  addRoute(predicate, service, nullptr);
}

void HTTPRouter::addRoute(
    std::function<bool (HTTPRequest*)> predicate,
    StreamingHTTPService* service,
    TaskScheduler* scheduler) {
  auto factory = [service, scheduler] (
      HTTPServerConnection* conn,
      HTTPRequest* req) -> std::unique_ptr<HTTPHandler> {
    auto handler = new HTTPServiceHandler(
        service,
        scheduler,
        conn,
        req);

    return std::unique_ptr<HTTPHandler>(handler);
  };

  routes_.emplace_back(predicate, factory);
}

void HTTPRouter::addRoute(
    std::function<bool (HTTPRequest*)> predicate,
    HTTPHandlerFactory* handler_factory) {

  auto factory = [handler_factory] (
      HTTPServerConnection* conn,
      HTTPRequest* req) ->std::unique_ptr<HTTPHandler> {
    return handler_factory->getHandler(conn, req);
  };

  routes_.emplace_back(predicate, factory);
}

std::unique_ptr<HTTPHandler> HTTPRouter::getHandler(
    HTTPServerConnection* conn,
    HTTPRequest* req) {
  for (const auto& route : routes_) {
    if (route.first(req)) {
      return route.second(conn, req);
    }
  }

  return std::unique_ptr<HTTPHandler>(new NoSuchRouteHandler(conn, req));
}

HTTPRouter::NoSuchRouteHandler::NoSuchRouteHandler(
    HTTPServerConnection* conn,
    HTTPRequest* req) :
    conn_(conn),
    req_(req) {
  res_.populateFromRequest(*req);
}

void HTTPRouter::NoSuchRouteHandler::handleHTTPRequest() {
  conn_->discardRequestBody([this] () {
    res_.setStatus(kStatusNotFound);
    res_.addBody("Not Found");

    conn_->writeResponse(
        res_,
        std::bind(&HTTPServerConnection::finishResponse, conn_),
        [] {});
  },
  [] {});
}

}
}
