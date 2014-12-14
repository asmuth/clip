/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/stringutil.h>
#include <fnord/base/uri.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/net/http/httpoutputstream.h>

namespace fnord {
namespace http {

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

void HTTPResponse::addCookie(
    const std::string& key,
    const std::string& value,
    const DateTime& expire /* = DateTime::epoch() */,
    const std::string& path /* = "" */,
    const std::string& domain /* = "" */,
    bool secure /* = false */,
    bool httponly /* = false */) {
  auto cookie_str = StringUtil::format(
      "$0=$1",
      URI::urlEncode(key),
      URI::urlEncode(value));

  if (static_cast<uint64_t>(expire) > 0) {
    cookie_str.append(StringUtil::format("; expires=$0",
        expire.toString("%a, %d-%b-%Y %H:%M:%S %Z")));
  }

  if (path.length() > 0) {
    cookie_str.append(StringUtil::format("; path=$0", path));
  }

  if (domain.length() > 0) {
    cookie_str.append(StringUtil::format("; domain=$0", domain));
  }

  if (httponly) {
    cookie_str.append("; httponly");
  }

  if (secure) {
    cookie_str.append("; secure");
  }

  // FIXPAUL appendheader
  setHeader("Set-Cookie", cookie_str);
}

void HTTPResponse::writeToOutputStream(HTTPOutputStream* output) {
  setHeader("Content-Length", std::to_string(body_.size()));
  output->writeStatusLine(version_, status_code_, status_);
  output->writeHeaders(headers_);
  output->getOutputStream()->write(body_);
}

void HTTPResponse::populateFromRequest(const HTTPRequest& request) {
  setVersion(request.getVersion());

  if (request.getVersion() == "HTTP/1.0") {
    if (request.keepalive()) {
      addHeader("Connection", "keep-alive");
    } else {
      addHeader("Connection", "close");
    }
  }
}

}
}
