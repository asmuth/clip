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
#include <metricd/util/exception.h>
#include <metricd/transport/http/cookies.h>
#include <metricd/transport/http/httpparser.h>
#include <metricd/transport/http/httprequest.h>

namespace fnordmetric {
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
    const Buffer& data) {
  return mkPost(uri, data, HTTPMessage::HeaderList{});
}

HTTPRequest HTTPRequest::mkPost(
    const URI& uri,
    const Buffer& data,
    const HTTPMessage::HeaderList& headers) {
  HTTPRequest req(HTTPMessage::M_POST, uri.pathAndQuery());
  req.addHeader("Host", uri.hostAndPort());

  for (const auto& hdr : headers) {
    req.setHeader(hdr.first, hdr.second);
  }

  req.addBody(data);
  return req;
}

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

  parser.onBodyChunk([&request] (const char* data, size_t size) {
    request.appendBody((char *) data, size);
  });

  parser.parse(str.c_str(), str.length());
  parser.eof();

  if (parser.state() != HTTPParser::S_DONE) {
    RAISE(kRuntimeError, "incomplete HTTP request");
  }

  return request;
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

}
}
