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
#ifndef _STX_HTTP_CLIENTCONNECTION_H
#define _STX_HTTP_CLIENTCONNECTION_H
#include <memory>
#include <vector>
#include <fnordmetric/transport/http/httphandler.h>
#include <fnordmetric/transport/http/httpparser.h>
#include <fnordmetric/transport/http/httprequest.h>
#include <fnordmetric/transport/http/httpresponse.h>
#include <fnordmetric/transport/http/httpstats.h>
#include <fnordmetric/util/net/inetaddr.h>
#include <fnordmetric/util/net/tcpconnection.h>
#include <fnordmetric/util/thread/taskscheduler.h>

namespace fnordmetric {
namespace http {
class HTTPResponseHandler;

class HTTPClientConnection {
public:
  static const size_t kMinBufferSize = 4096;

  HTTPClientConnection(
      std::unique_ptr<net::TCPConnection> conn,
      TaskScheduler* scheduler,
      HTTPClientStats* stats);

  ~HTTPClientConnection();

  HTTPClientConnection(const HTTPClientConnection& other) = delete;
  HTTPClientConnection& operator=(const HTTPClientConnection& other) = delete;

  void executeRequest(
      const HTTPRequest& request,
      HTTPResponseHandler* response_handler);

  Wakeup* onReady();

  bool isIdle() const;

protected:

  enum kHTTPClientConnectionState {
    S_CONN_IDLE,
    S_CONN_BUSY,
    S_CONN_CLOSED
  };

  void read();
  void write();
  void awaitRead();
  void awaitWrite();
  void close();
  void keepalive();

  void error(const std::exception& e);

  std::unique_ptr<net::TCPConnection> conn_;
  TaskScheduler* scheduler_;
  kHTTPClientConnectionState state_;
  HTTPParser parser_;
  Buffer buf_;
  std::mutex mutex_;
  HTTPResponseHandler* cur_handler_;
  Wakeup on_ready_;
  bool keepalive_;
  HTTPClientStats* stats_;
};

}
}
#endif
