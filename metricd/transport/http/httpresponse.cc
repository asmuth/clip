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
#include <metricd/util/inspect.h>
#include <metricd/util/stringutil.h>
#include <metricd/util/uri.h>
#include <metricd/transport/http/cookies.h>
#include <metricd/transport/http/httpresponse.h>
#include <metricd/transport/http/httpparser.h>

namespace fnordmetric {
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
    const UnixTime& expire /* = UnixTime::epoch() */,
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
