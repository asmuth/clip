/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnord/net/http/cookies.h>
#include <fnord/net/http/httpparser.h>
#include <fnord/net/http/httprequest.h>

namespace fnord {
namespace http {

HTTPRequest HTTPRequest::parse(const std::string& str) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_REQUEST);
  HTTPRequest request;

  parser.onMethod([&request] (HTTPMessage::kHTTPMethod method) {
    request.setMethod(method);
  });

  parser.onURI([&request] (const char* data, size_t size) {
    request.setURI(std::string(data, size));
  });

  parser.onVersion([&request] (const char* data, size_t size) {
    request.setVersion(std::string(data, size));
  });

  parser.onHeader([&request] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    request.addHeader(std::string(key, key_size), std::string(val, val_size));
  });

  parser.parse(str.c_str(), str.length());
  parser.eof();

  if (parser.state() != HTTPParser::S_DONE) {
    RAISE(kRuntimeError, "incomplete HTTP request");
  }

  return request;
}

HTTPRequest::HTTPRequest() : method_(HTTPMessage::M_GET) {}

HTTPMessage::kHTTPMethod HTTPRequest::method() const {
  return method_;
}

void HTTPRequest::setMethod(HTTPMessage::kHTTPMethod method) {
  method_ = method;
}

const std::string& HTTPRequest::uri() const {
  return url_;
}

void HTTPRequest::setURI(const std::string& uri) {
  url_ = uri;
}

const bool HTTPRequest::keepalive() const {
  if (version() == "HTTP/1.1") {
    return true;
  }

  if (getHeader("Connection") == "keep-alive") {
    return true;
  }

  return false;
}

std::vector<std::pair<std::string, std::string>> HTTPRequest::cookies() const {
  return Cookies::parseCookieHeader(getHeader("Cookie"));
}

}
}
