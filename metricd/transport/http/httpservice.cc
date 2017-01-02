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
#include <metricd/util/inspect.h>
#include <metricd/util/logging.h>
#include "metricd/transport/http/httpserverconnection.h"
#include <metricd/transport/http/httpservice.h>
#include <metricd/transport/http/HTTPResponseStream.h>

namespace fnordmetric {
namespace http {

void HTTPService::handleHTTPRequest(
    RefPtr<HTTPRequestStream> req_stream,
    RefPtr<HTTPResponseStream> res_stream) {
  const auto& req = req_stream->request();

  HTTPResponse res;
  res.populateFromRequest(req);

  handleHTTPRequest(const_cast<HTTPRequest*>(&req), &res);

  auto body_size = res.body().size();
  if (body_size > 0) {
    res.setHeader("Content-Length", StringUtil::toString(body_size));
  }

  res_stream->writeResponse(res);
}

HTTPServiceHandler::HTTPServiceHandler(
    StreamingHTTPService* service,
    TaskScheduler* scheduler,
    HTTPServerConnection* conn,
    HTTPRequest* req) :
    service_(service),
    scheduler_(scheduler),
    conn_(conn),
    req_(req) {}

void HTTPServiceHandler::handleHTTPRequest() {
  if (service_->isStreaming()) {
    dispatchRequest();
  } else {
    conn_->readRequestBody([this] (
        const void* data,
        size_t size,
        bool last_chunk) {
      req_->appendBody((char *) data, size);

      if (last_chunk) {
        dispatchRequest();
      }
    },
    [] {});
  }
}

void HTTPServiceHandler::dispatchRequest() {
  auto runnable = [this] () {
    auto res_stream = mkRef(new HTTPResponseStream(conn_));
    auto req_stream = mkRef(new HTTPRequestStream(*req_, conn_));

    try {
      service_->handleHTTPRequest(req_stream, res_stream);
    } catch (const std::exception& e) {
      logError("Error while processing HTTP request: $0", e);

      try {
        if (res_stream->isOutputStarted()) {
          res_stream->finishResponse();
        } else {
          http::HTTPResponse res;
          res.populateFromRequest(req_stream->request());
          res.setStatus(http::kStatusInternalServerError);
          res.addBody("server error");
          res_stream->writeResponse(res);
        }
      } catch (const std::exception& e) {
        logError("Connection Error: $0", e);
      }
    }
  };

  if (scheduler_ == nullptr) {
    runnable();
  } else {
    scheduler_->run(runnable);
  }
}

}
}

