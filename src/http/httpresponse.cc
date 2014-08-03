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

HTTPResponse::HTTPResponse() :
    version_("HTTP/1.1"),
    status_(400) {}

void HTTPResponse::setStatus(int status) {
  status_ = status;
}

void HTTPResponse::addHeader(const std::string& key, const std::string& value) {
  headers_.emplace_back(key, value);
}

void HTTPResponse::addBody(const std::string& body) {
  body_ = body;
  addHeader("Content-Length", std::to_string(body_.size()));
}

void HTTPResponse::writeToOutputStream(HTTPOutputStream* output) {
  output->writeStatusLine(version_, status_);
  output->writeHeaders(headers_);
  output->getOutputStream()->write(body_);
}

}
}
