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
#include <libtransport/uri/uri.h>
#include <libtransport/http/cookies.h>
#include <libtransport/http/http_response.h>
#include <libtransport/http/v1/http_parser.h>

namespace libtransport {
namespace http {

bool HTTPResponse::parse(const std::string& str) {
  return parse(str.data(), str.size());
}

bool HTTPResponse::parse(const char* str, size_t strlen) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_RESPONSE);

  parser.onVersion([this] (const char* data, size_t size) {
    setVersion(std::string(data, size));
  });

  parser.onStatusName([this] (const char* data, size_t size) {
    setStatusName(std::string(data, size));
  });

  parser.onStatusCode([this] (int code) {
    setStatusCode(code);
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

HTTPResponse::HTTPResponse() {
  setStatus(kStatusNotFound);
}

void HTTPResponse::setStatus(int status_code, const std::string& status) {
  status_code_ = status_code;
  status_ = status;
}

void HTTPResponse::setStatus(const HTTPStatus& status) {
  setStatus(status.code, status.name);
}

void HTTPResponse::setStatusCode(int code) {
  status_code_ = code;
}

void HTTPResponse::setStatusName(const std::string& status) {
  status_ = status;
}

int HTTPResponse::statusCode() const {
  return status_code_;
}

const std::string& HTTPResponse::statusName() const {
  return status_;
}

void HTTPResponse::addCookie(
    const std::string& key,
    const std::string& value,
    uint64_t expires_timestamp /* = 0 */,
    const std::string& path /* = "" */,
    const std::string& domain /* = "" */,
    bool secure /* = false */,
    bool httponly /* = false */) {
  auto cookie_str = Cookies::mkCookie(
      key,
      value,
      expires_timestamp,
      path,
      domain,
      secure,
      httponly);

  // FIXPAUL appendheader
  setHeader("Set-Cookie", cookie_str);
}

void HTTPResponse::populateFromRequest(const HTTPRequest& request) {
  const auto& version = request.version();

  setVersion(version);

  if (version == "HTTP/1.0") {
    if (request.keepalive()) {
      addHeader("Connection", "keep-alive");
    } else {
      addHeader("Connection", "close");
    }
  }
}

} // namespace http
} // namespace libtransport

