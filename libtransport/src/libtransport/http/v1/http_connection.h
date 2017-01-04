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
#include <memory>
#include <vector>
#include <libtransport/http/http_request.h>
#include <libtransport/http/http_response.h>

namespace libtransport {
namespace http {

class HTTPConnection {
public:

  HTTPConnection(
      int fd,
      uint64_t io_timeout,
      const std::string& prelude_bytes = "");

  ~HTTPConnection();

  bool sendRequest(const HTTPRequest& req);
  bool sendRequestHeaders(const HTTPRequest& req);
  bool sendRequestBodyChunk(const char* data, size_t size);
  bool recvRequest(HTTPRequest* request);

  bool sendResponse(const HTTPResponse& res);
  bool sendResponseHeaders(const HTTPResponse& res);
  bool sendResponseBodyChunk(const char* data, size_t size);
  bool recvResponse(HTTPResponse* response);

  bool isClosed() const;
  void close();

  std::string getError() const;

protected:

  bool write(const char* data, size_t size);

  void setError(const std::string& error);

  int fd_;
  uint64_t io_timeout_;
  std::string prelude_bytes_;
  bool closed_;
  std::string error_;
};

} // namespace http
} // namespace libtransport

