/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httprequest.h>

using fnordmetric::util::InputStream;
using fnordmetric::util::OutputStream;
using fnordmetric::util::StringInputStream;
using fnordmetric::util::StringOutputStream;

namespace fnord {
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
  auto key_low = key;
  std::transform(key_low.begin(), key_low.end(), key_low.begin(), ::tolower);

  for (const auto& header : headers_) {
    if (header.first == key_low) {
      return header.second;
    }
  }

  return kEmptyHeader;
}

void HTTPMessage::addHeader(const std::string& key, const std::string& value) {
  auto key_low = key;
  std::transform(key_low.begin(), key_low.end(), key_low.begin(), ::tolower);
  headers_.emplace_back(key_low, value);
}

void HTTPMessage::setHeader(const std::string& key, const std::string& value) {
  auto key_low = key;
  std::transform(key_low.begin(), key_low.end(), key_low.begin(), ::tolower);

  for (auto& header : headers_) {
    if (header.first == key_low) {
      header.second = value;
      return;
    }
  }

  headers_.emplace_back(key_low, value);
}

const std::string& HTTPMessage::getBody() const {
  return body_;
}

void HTTPMessage::addBody(const std::string& body) {
  body_ = body;
  setHeader("Content-Length", std::to_string(body.size()));
}

void HTTPMessage::clearBody() {
  body_.clear();
  // FIXPAUL remove Content-Length header
}

std::unique_ptr<InputStream> HTTPMessage::getBodyInputStream() const {
  return StringInputStream::fromString(body_);
}

std::unique_ptr<OutputStream> HTTPMessage::getBodyOutputStream() {
  return StringOutputStream::fromString(&body_);
}

}
}
