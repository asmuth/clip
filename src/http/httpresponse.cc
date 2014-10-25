/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httpresponse.h>
#include <fnordmetric/http/httpoutputstream.h>

namespace fnordmetric {
namespace http {

HTTPResponse::HTTPResponse() {
  setStatus(kStatusNotFound);
}

void HTTPResponse::setStatus(int status_code, const std::string& status) {
  status_code = status_code_;
  status_ = status;
}

void HTTPResponse::setStatus(const HTTPStatus& status) {
  setStatus(status.code, status.name);
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
