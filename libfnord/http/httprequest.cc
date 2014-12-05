/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/http/httprequest.h>
#include <fnord/http/httpinputstream.h>

namespace fnord {
namespace http {

HTTPRequest::HTTPRequest() {}

HTTPRequest::HTTPRequest(
    const std::string& method,
    const std::string& url) :
    method_(method),
    url_(url) {}

const std::string& HTTPRequest::getMethod() const {
  return method_;
}

// FIXPAUL sloooow
HTTPRequest::kMethod HTTPRequest::method() const {
  if (method_ == "CONNECT") { return M_CONNECT; }
  if (method_ == "DELETE") { return M_DELETE; }
  if (method_ == "GET") { return M_GET; }
  if (method_ == "HEAD") { return M_HEAD; }
  if (method_ == "OPTIONS") { return M_OPTIONS; }
  if (method_ == "POST") { return M_POST; }
  if (method_ == "PUT") { return M_PUT; }
  if (method_ == "TRACE") { return M_TRACE; }
  return M_INVALID;
}

const std::string& HTTPRequest::getUrl() const {
  return url_;
}

const bool HTTPRequest::keepalive() const {
  if (getVersion() == "HTTP/1.1") {
    return true;
  }

  if (getHeader("Connection") == "keep-alive") {
    return true;
  }

  return false;
}

void HTTPRequest::readFromInputStream(HTTPInputStream* input) {
  input->readStatusLine(&method_, &url_, &version_);
  input->readHeaders(&headers_);

  const auto& content_length_header = getHeader("Content-Length");
  int content_length = 0;
  if (content_length_header.size() > 0) {
    try {
      content_length = std::stoi(content_length_header);
    } catch (std::exception e){
      /* invalid content length */
    }
  }

  if (content_length > 0) {
    input->getInputStream()->readNextBytes(&body_, content_length);
  }
}

}
}
