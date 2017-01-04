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
#pragma once
#include <functional>
#include "libtransport/http/http_request.h"
#include "libtransport/http/v1/http_connection.h"

namespace libtransport {
namespace http {

class HTTPServer {
public:

  using ConnectionCallbackType =
      std::function<void (std::unique_ptr<HTTPConnection>)>;

  using RequestCallbackType =
      std::function<void (HTTPRequest*, HTTPResponse*)>;

  HTTPServer();

  bool listen(const std::string& addr, int port);
  bool run();

  void setConnectionHandler(ConnectionCallbackType callback);
  void setRequestHandler(RequestCallbackType);

  static void handleConnection(
      std::unique_ptr<HTTPConnection> connection,
      RequestCallbackType request_handler);

protected:
  int ssock_;
  uint64_t io_timeout_;
  ConnectionCallbackType connection_handler_;
};

} // namespace http
} // namespace libtransport

