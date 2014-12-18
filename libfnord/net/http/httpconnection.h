/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_CONNECTION_H
#define _FNORD_HTTP_CONNECTION_H
#include <memory>
#include <vector>
#include <fnord/logging/logger.h>
#include <fnord/net/http/httpparser.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/net/tcpconnection.h>
#include <fnord/thread/taskscheduler.h>

namespace fnord {
namespace http {

class HTTPConnection {
public:
  HTTPConnection(
      std::unique_ptr<net::TCPConnection> conn,
      thread::TaskScheduler* server_scheduler,
      thread::TaskScheduler* request_scheduler);

  void onRequest(std::function<void (HTTPRequest* req)> callback);

  void readRequestBody(
      std::function<void (const void* data, size_t size, bool last_chunk)>);

  void writeResponse(const HTTPResponse& resp);

  void writeResponseHeaders(
      const HTTPResponse& resp,
      std::function<void()> ready_callback);

  void writeResponseBody(
      const void* data,
      size_t size,
      std::function<void()> ready_callback);

  void finishResponse();

protected:
  void read();
  void write();
  void dispatchRequest();
  void readNextRequestHeaders();

  void incRef();
  void decRef();
  void close();

  std::unique_ptr<net::TCPConnection> conn_;
  thread::TaskScheduler* server_scheduler_;
  thread::TaskScheduler* request_scheduler_;

  HTTPParser parser_;
  std::unique_ptr<HTTPRequest> cur_request_;
  std::function<void (HTTPRequest* req)> on_request_cb_;
  std::function<void ()> on_read_completed_cb_;
  std::function<void ()> on_write_completed_cb_;
  std::atomic<int> refcount_;
  Buffer buf_;
};

}
}
#endif
