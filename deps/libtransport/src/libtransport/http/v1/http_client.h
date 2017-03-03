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
#include <memory>
#include "libtransport/http/http_request.h"
#include "libtransport/http/http_response.h"
#include "libtransport/http/v1/http_connection.h"

namespace libtransport {
namespace http {

class HTTPClient {
public:

  using BodyChunkCallback = std::function<void (const char*, size_t)>;

  HTTPClient();

  bool connect(const std::string& hostname, uint16_t port);

  bool executeRequest(const HTTPRequest& req, HTTPResponse* res);

  bool executeRequest(
      const HTTPRequest& req,
      HTTPResponse* res,
      BodyChunkCallback on_body_chunk);

  std::string getError() const;

protected:
  std::unique_ptr<HTTPConnection> conn_;
  uint64_t io_timeout_;
  std::string error_;
};

} // namespace http
} // namespace libtransport

