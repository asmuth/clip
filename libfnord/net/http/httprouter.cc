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
#include "fnord/net/http/httpserverconnection.h"
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpservice.h"

namespace fnord {
namespace http {

void HTTPRouter::addRoute(
    std::function<bool (HTTPRequest*)> predicate,
    HTTPService* service) {
  addRoute(predicate, service, nullptr);
}

void HTTPRouter::addRoute(
    std::function<bool (HTTPRequest*)> predicate,
    HTTPService* service,
    thread::TaskScheduler* scheduler) {
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
        std::bind(&HTTPServerConnection::finishResponse, conn_));
  });
}

}
}

