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
#ifndef _STX_HTTP_SERVERCONNECTION_H
#define _STX_HTTP_SERVERCONNECTION_H
#include <memory>
#include <vector>
#include <fnordmetric/util/autoref.h>
#include <fnordmetric/util/stdtypes.h>
#include <fnordmetric/transport/http/httphandler.h>
#include <fnordmetric/transport/http/httpparser.h>
#include <fnordmetric/transport/http/httprequest.h>
#include <fnordmetric/transport/http/httpresponse.h>
#include <fnordmetric/transport/http/httpstats.h>
#include <fnordmetric/transport/http/tcpconnection.h>
#include <fnordmetric/transport/http/taskscheduler.h>

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
