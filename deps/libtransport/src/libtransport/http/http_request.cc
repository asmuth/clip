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
#include <libtransport/http/cookies.h>
#include <libtransport/http/v1/http_parser.h>
#include <libtransport/http/http_request.h>

namespace libtransport {
namespace http {

HTTPRequest HTTPRequest::mkGet(const std::string& uri) {
  return mkGet(URI(uri), HTTPMessage::HeaderList{});
}

HTTPRequest HTTPRequest::mkGet(
    const std::string& uri,
    const HTTPMessage::HeaderList& headers) {
  return mkGet(URI(uri), headers);
}

HTTPRequest HTTPRequest::mkGet(const URI& uri) {
  return mkGet(uri, HTTPMessage::HeaderList{});
}

HTTPRequest HTTPRequest::mkGet(
    const URI& uri,
    const HTTPMessage::HeaderList& headers) {
  HTTPRequest req(HTTPMessage::M_GET, uri.pathAndQuery());
  req.addHeader("Host", uri.hostAndPort());

  for (const auto& hdr : headers) {
    req.setHeader(hdr.first, hdr.second);
  }

  return req;
}

HTTPRequest HTTPRequest::mkPost(
    const URI& uri,
    const std::string& data) {
  return mkPost(uri, data, HTTPMessage::HeaderList{});
}

HTTPRequest HTTPRequest::mkPost(
    const URI& uri,
    const std::string& data,
    const HTTPMessage::HeaderList& headers) {
  HTTPRequest req(HTTPMessage::M_POST, uri.pathAndQuery());
  req.addHeader("Host", uri.hostAndPort());

  for (const auto& hdr : headers) {
    req.setHeader(hdr.first, hdr.second);
  }

  req.addBody(data);
  return req;
}

bool HTTPRequest::parse(const std::string& str) {
  return parse(str.data(), str.size());
}

bool HTTPRequest::parse(const char* str, size_t strlen) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_REQUEST);

  parser.onMethod([this] (HTTPMessage::kHTTPMethod method) {
    setMethod(method);
  });

  parser.onURI([this] (const char* data, size_t size) {
    setURI(std::string(data, size));
  });

  parser.onVersion([this] (const char* data, size_t size) {
    setVersion(std::string(data, size));
  });

  parser.onHeader([this] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    addHeader(std::string(key, key_size), std::string(val, val_size));
  });

  parser.onBodyChunk([this] (const char* data, size_t size) {
    appendBody((char *) data, size);
  });

  if (!parser.parse(str, strlen)) {
    return false;
  }

  if (!parser.eof()) {
    return false;
  }

  if (parser.state() != HTTPParser::S_DONE) {
    return false;
  }

  return true;
}

HTTPRequest::HTTPRequest() : method_(HTTPMessage::M_GET) {}

HTTPRequest::HTTPRequest(
    kHTTPMethod method,
    const std::string& uri) :
    method_(method),
    url_(uri) {
  setVersion("HTTP/1.1");
}

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

bool HTTPRequest::keepalive() const {
  if (getHeader("Connection") == "keep-alive") {
    return true;
  }

  if (getHeader("Connection") == "close") {
    return false;
  }

  if (version() == "HTTP/1.1") {
    return true;
  } else {
    return false;
  }
}

std::vector<std::pair<std::string, std::string>> HTTPRequest::cookies() const {
  return Cookies::parseCookieHeader(getHeader("Cookie"));
}

} // namespace http
} // namespace libtransport

