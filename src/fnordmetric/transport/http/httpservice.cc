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
#include <fnordmetric/util/inspect.h>
#include <fnordmetric/util/logging.h>
#include "fnordmetric/transport/http/httpserverconnection.h"
#include <fnordmetric/transport/http/httpservice.h>
#include <fnordmetric/transport/http/HTTPResponseStream.h>

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
      logError("http.server", e, "Error while processing HTTP request");

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
        logError("http.server", e, "Connection Error");
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

