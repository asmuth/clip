/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_SERVERCONNECTION_H
#define _FNORD_HTTP_SERVERCONNECTION_H
#include <memory>
#include <vector>
#include <fnord/base/autoref.h>
#include <fnord/base/stdtypes.h>
#include <fnord/logging/logger.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/net/http/httpparser.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/net/tcpconnection.h>
#include <fnord/thread/taskscheduler.h>

namespace fnord {
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
      UniqueRef<net::TCPConnection> conn,
      TaskScheduler* scheduler);

  void readRequestBody(
      Function<void (
          const void* data,
          size_t size,
          bool last_chunk)> callback);

  void discardRequestBody(
      Function<void ()> ready_callback);

  void writeResponse(
      const HTTPResponse& resp,
      Function<void()> ready_callback);

  void writeResponseBody(
      const void* data,
      size_t size,
      Function<void()> ready_callback);

  void finishResponse();

protected:
  HTTPServerConnection(
      HTTPHandlerFactory* handler_factory,
      UniqueRef<net::TCPConnection> conn,
      TaskScheduler* scheduler);

  void nextRequest();
  void dispatchRequest();

  void read();
  void write();
  void awaitRead();
  void awaitWrite();
  void close();

  HTTPHandlerFactory* handler_factory_;
  UniqueRef<net::TCPConnection> conn_;
  TaskScheduler* scheduler_;
  HTTPParser parser_;
  Function<void ()> on_write_completed_cb_;
  Buffer buf_;
  Buffer body_buf_;
  UniqueRef<HTTPRequest> cur_request_;
  UniqueRef<HTTPHandler> cur_handler_;
  std::mutex mutex_;
};

}
}
#endif
