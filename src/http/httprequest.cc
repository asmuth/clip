/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httpinputstream.h>
#include <fnordmetric/http/httprequest.h>

namespace fnordmetric {
namespace http {

void HTTPRequest::readFromInputStream(HTTPInputStream* input) {
  input->readStatusLine(&method_, &url_, &version_);
  input->readHeaders(&headers_);
}

const std::string& HTTPRequest::getUrl() const {
  return url_;
}

const std::vector<std::pair<std::string, std::string>>&
    HTTPRequest::getHeaders() const {
  return headers_;
}

}
}
