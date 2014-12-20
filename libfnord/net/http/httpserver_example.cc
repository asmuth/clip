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
#include <unistd.h>
#include "fnord/base/exception.h"
#include "fnord/base/exceptionhandler.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/net/http/httphandler.h"
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpservice.h"
#include "fnord/logging/logger.h"
#include "fnord/logging/logoutputstream.h"
#include "fnord/thread/threadpool.h"
#include "fnord/system/signalhandler.h"

/**
 * Example 1: A simple HTTP Service
 */
class TestService : public fnord::http::HTTPService {

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res) {
    auto res_body = fnord::StringUtil::format(
        "pong: $0",
        req->body().toString());

    res->setStatus(fnord::http::kStatusOK);
    res->addBody(res_body);
  }

};

/**
 * Example 2: A streaming HTTP Handler
 */
class StreamingTestHandler : public fnord::http::HTTPHandler {
public:
  StreamingTestHandler(
      fnord::http::HTTPConnection* conn,
      fnord::http::HTTPRequest* req) :
      conn_(conn),
      req_(req),
      body_len_(0),
      chunks_written_(0) {
    res_.populateFromRequest(*req);
  }

  void handleHTTPRequest() override {
    conn_->readRequestBody([this] (
        const void* data,
        size_t size,
        bool last_chunk) {
      body_len_ += size;

      if (last_chunk) {
        writeResponseHeaders();
      }
    });
  }

  void writeResponseHeaders() {
    res_.setStatus(fnord::http::kStatusOK);
    res_.addHeader("Content-Length", fnord::StringUtil::toString(5 * 10));
    res_.addHeader("X-Orig-Bodylen", fnord::StringUtil::toString(body_len_));

    conn_->writeResponse(
        res_,
        std::bind(&StreamingTestHandler::writeResponseBodyChunk, this));
  }

  void writeResponseBodyChunk() {
    usleep(100000); // sleep for demo

    std::string buf = "blah\n";
    if (++chunks_written_ == 10) {
      conn_->writeResponseBody(
          buf.c_str(),
          buf.length(),
          std::bind(&fnord::http::HTTPConnection::finishResponse, conn_));
    } else {
      conn_->writeResponseBody(
          buf.c_str(),
          buf.length(),
          std::bind(&StreamingTestHandler::writeResponseBodyChunk, this));
    }
  }

protected:
  size_t body_len_;
  int chunks_written_;
  fnord::http::HTTPConnection* conn_;
  fnord::http::HTTPRequest* req_;
  fnord::http::HTTPResponse res_;
};

class StreamingTestHandlerFactory : public fnord::http::HTTPHandlerFactory {
public:
  std::unique_ptr<fnord::http::HTTPHandler> getHandler(
      fnord::http::HTTPConnection* conn,
      fnord::http::HTTPRequest* req) override {
    return std::unique_ptr<fnord::http::HTTPHandler>(
        new StreamingTestHandler(conn, req));
  }
};

int main() {
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  fnord::CatchAndAbortExceptionHandler ehandler;
  ehandler.installGlobalHandlers();

  fnord::log::LogOutputStream logger(fnord::io::OutputStream::getStderr());
  fnord::log::Logger::get()->setMinimumLogLevel(fnord::log::kInfo);
  fnord::log::Logger::get()->listen(&logger);

  fnord::thread::ThreadPool thread_pool;
  fnord::http::HTTPRouter router;
  fnord::http::HTTPServer http_server(&router, &thread_pool);

  TestService ping_example;
  router.addRouteByPrefixMatch("/ping", &ping_example, &thread_pool);

  StreamingTestHandlerFactory streaming_example;
  router.addRouteByPrefixMatch("/stream", &streaming_example);

  http_server.listen(8080);

  for (;;) { usleep(100000); }

  return 0;
}

