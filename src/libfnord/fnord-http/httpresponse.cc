/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/exception.h>
#include <fnord-base/inspect.h>
#include <fnord-base/stringutil.h>
#include <fnord-base/uri.h>
#include <fnord-http/cookies.h>
#include <fnord-http/httpresponse.h>
#include <fnord-http/httpparser.h>

namespace fnord {
namespace http {

HTTPResponse HTTPResponse::parse(const std::string& str) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_RESPONSE);
  HTTPResponse response;

  parser.onVersion([&response] (const char* data, size_t size) {
    response.setVersion(std::string(data, size));
  });

  parser.onStatusName([&response] (const char* data, size_t size) {
    response.setStatusName(std::string(data, size));
  });

  parser.onStatusCode([&response] (int code) {
    response.setStatusCode(code);
  });

  parser.onHeader([&response] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    response.addHeader(std::string(key, key_size), std::string(val, val_size));
  });

  parser.onBodyChunk([&response] (const char* data, size_t size) {
    response.appendBody((char *) data, size);
  });

  parser.parse(str.c_str(), str.length());
  parser.eof();

  if (parser.state() != HTTPParser::S_DONE) {
    RAISE(kRuntimeError, "incomplete HTTP response");
  }

  return response;
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
    const DateTime& expire /* = DateTime::epoch() */,
    const std::string& path /* = "" */,
    const std::string& domain /* = "" */,
    bool secure /* = false */,
    bool httponly /* = false */) {
  auto cookie_str = Cookies::mkCookie(
      key,
      value,
      expire,
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

}
}
