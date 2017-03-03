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
#include <libtransport/http/http_message.h>
#include <libtransport/http/http_request.h>
#include <libtransport/http/http_status.h>
#include <string>

namespace libtransport {
namespace http {

class HTTPResponse : public HTTPMessage {
public:

  HTTPResponse();

  void setStatus(int status_code, const std::string& status);
  void setStatus(const HTTPStatus& status);
  void setStatusCode(int code);
  void setStatusName(const std::string& status);

  void addCookie(
      const std::string& key,
      const std::string& value,
      uint64_t expires_timestamp = 0,
      const std::string& path = "",
      const std::string& domain = "",
      bool secure = false,
      bool httponly = false);

  void populateFromRequest(const HTTPRequest& request);

  bool parse(const std::string& str);
  bool parse(const char* str, size_t strlen);

  int statusCode() const;
  const std::string& statusName() const;

protected:
  int status_code_;
  std::string status_;
};

} // namespace http
} // namespace libtransport

