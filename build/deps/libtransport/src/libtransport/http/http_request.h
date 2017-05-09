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
#include <libtransport/uri/uri.h>
#include <libtransport/http/http_message.h>
#include <string>

namespace libtransport {
namespace http {

class HTTPRequest : public HTTPMessage {
public:

  static HTTPRequest mkGet(const std::string& uri);

  static HTTPRequest mkGet(
      const std::string& uri,
      const HTTPMessage::HeaderList& headers);

  static HTTPRequest mkGet(const URI& uri);

  static HTTPRequest mkGet(
      const URI& uri,
      const HTTPMessage::HeaderList& headers);

  static HTTPRequest mkPost(
      const URI& uri,
      const std::string& data);

  static HTTPRequest mkPost(
      const URI& uri,
      const std::string& data,
      const HTTPMessage::HeaderList& headers);

  HTTPRequest();
  HTTPRequest(kHTTPMethod method, const std::string& uri);

  kHTTPMethod method() const;
  void setMethod(kHTTPMethod method);

  const std::string& uri() const;
  void setURI(const std::string& uri);

  bool keepalive() const;
  std::vector<std::pair<std::string, std::string>> cookies() const;

  bool parse(const char* str, size_t strlen);
  bool parse(const std::string& str);

protected:
  kHTTPMethod method_;
  std::string url_;
};

} // namespace http
} // namespace libtransport

