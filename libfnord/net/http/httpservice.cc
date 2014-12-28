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
#include "fnord/net/http/httpserverconnection.h"
#include <fnord/net/http/httpservice.h>

namespace fnord {
namespace http {

HTTPServiceHandler::HTTPServiceHandler(
    HTTPService* service,
    thread::TaskScheduler* scheduler,
    HTTPServerConnection* conn,
    HTTPRequest* req) :
    service_(service),
    scheduler_(scheduler),
    conn_(conn),
    req_(req) {
  res_.populateFromRequest(*req);
}

void HTTPServiceHandler::handleHTTPRequest() {
  conn_->readRequestBody([this] (
      const void* data,
      size_t size,
      bool last_chunk) {
    req_->appendBody((char *) data, size);

    if (last_chunk) {
      dispatchRequest();
    }
  });
}

void HTTPServiceHandler::dispatchRequest() {
  auto runnable = [this] () {
    try {
      service_->handleHTTPRequest(req_, &res_);

      auto body_size = res_.body().size();
      if (body_size > 0) {
        res_.setHeader("Content-Length", StringUtil::toString(body_size));
      }
    } catch (const std::exception& e) {
      log::Logger::get()->logException(
          log::kError,
          "Error while processing HTTP request",
          e);

      res_.setStatus(http::kStatusInternalServerError);
      res_.clearBody();
      res_.addBody("server error");
    }

    conn_->writeResponse(
        res_,
        std::bind(&HTTPServerConnection::finishResponse, conn_));
  };

  if (scheduler_ == nullptr) {
    runnable();
  } else {
    scheduler_->run(runnable);
  }
}

}
}

