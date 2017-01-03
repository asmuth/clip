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
#ifndef _STX_HTTP_SERVERCONNECTION_H
#define _STX_HTTP_SERVERCONNECTION_H
#include <memory>
#include <vector>
#include <metricd/util/autoref.h>
#include <metricd/util/stdtypes.h>
#include <metricd/transport/http/httphandler.h>
#include <metricd/transport/http/httpparser.h>
#include <metricd/transport/http/httprequest.h>
#include <metricd/transport/http/httpresponse.h>
#include <metricd/transport/http/httpstats.h>
#include <metricd/transport/http/tcpconnection.h>
#include <metricd/transport/http/taskscheduler.h>

namespace fnordmetric {
namespace http {

class HTTPServerConnection : public RefCounted {
public:
  static const size_t kMinBufferSize = 4096;

  /**
   * Start a new HTTP connection. conn must be an opened and valid TCP
   * connection.
   *
   * This method does not return a reference to the newly created HTTP
   * connection to simplify connection lifetime management. The HTTP connection
   * will free itself once it is closed.
   *
   * The user must provide a callback that is executed for each incoming
   * request. A pointer to the HTTP connection will be passed to the onRequest
   * callback and is valid only until the callback returns. Therefore it is
   * crucial that the passed connection pointer is only dereferenced from the
   * onRequest callback and not store anywhere.
   *
   * Note that the onRequest callback will be called after all headers are read.
   * The onRequest callback _must_ call {read,discard}RequestBody to read or
   * discard the remainder of the request, it then _must_ call one of the
   * writeReponse methods to return a HTTP response to the user and then _must_
   * call the finishResponse method from the callback that is passed to the
   * writeResponse methods.
   *
   * Here is a simple example:
   *
   *    HTTPServerConnection::start(
   *         ...,
   *         ...,
   *         [] (HTTPServerConnection* conn, HTTPRequest* req) {
   *           conn->discardRequestBody([conn] () {
   *             HTTPResponse response;
   *             response.setStatus(kStatusOK);
   *             response.addBody("pong");
   *
   *             conn->writeResponse(
   *                 response,
   *                 std::bind(&HTTPServerConnection::finishResponse, conn));
   *           });
   *         });
   *
   **/
  static void start(
      HTTPHandlerFactory* handler_factory,
      ScopedPtr<TCPConnection> conn,
      TaskScheduler* scheduler,
      HTTPServerStats* stats);

  ~HTTPServerConnection();

  void readRequestBody(
      Function<void (
          const void* data,
          size_t size,
          bool last_chunk)> callback,
      Function<void()> on_error);

  void discardRequestBody(
      Function<void ()> ready_callback,
      Function<void()> on_error);

  void writeResponse(
      const HTTPResponse& resp,
      Function<void()> ready_callback,
      Function<void()> on_error);

  void writeResponseBody(
      const void* data,
      size_t size,
      Function<void()> ready_callback,
      Function<void()> on_error);

  void finishResponse();

  bool isClosed() const;

protected:
  HTTPServerConnection(
      HTTPHandlerFactory* handler_factory,
      ScopedPtr<TCPConnection> conn,
      TaskScheduler* scheduler,
      HTTPServerStats* stats);

  void nextRequest();
  void dispatchRequest();

  void read();
  void write();
  void awaitRead();
  void awaitWrite();
  void close();

  HTTPHandlerFactory* handler_factory_;
  ScopedPtr<TCPConnection> conn_;
  TaskScheduler* scheduler_;
  HTTPParser parser_;
  Function<void ()> on_write_completed_cb_;
  Function<void ()> on_error_cb_;
  Buffer read_buf_;
  Buffer write_buf_;
  Buffer body_buf_;
  ScopedPtr<HTTPRequest> cur_request_;
  ScopedPtr<HTTPHandler> cur_handler_;
  mutable std::recursive_mutex mutex_;
  bool closed_;
  HTTPServerStats* stats_;
};

}
}
#endif
