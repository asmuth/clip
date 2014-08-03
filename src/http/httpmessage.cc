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

std::string HTTPMessage::kEmptyHeader = "";

const std::string& HTTPMessage::getVersion() const {
  return version_;
}

void HTTPMessage::setVersion(const std::string& version) {
  version_ = version;
}

const std::vector<std::pair<std::string, std::string>>&
    HTTPMessage::getHeaders() const {
  return headers_;
}

const std::string& HTTPMessage::getHeader(const std::string& key) const {
  for (const auto& header : headers_) {
    if (header.first == key) {
      return header.second;
    }
  }

  return kEmptyHeader;
}

void HTTPMessage::addHeader(const std::string& key, const std::string& value) {
  headers_.emplace_back(key, value);
}

const std::string& HTTPMessage::getBody() const {
  return body_;
}

void HTTPMessage::addBody(const std::string& body) {
  body_ = body;
  addHeader("Content-Length", std::to_string(body_.size()));
}

}
}
