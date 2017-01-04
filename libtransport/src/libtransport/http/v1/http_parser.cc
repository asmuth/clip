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
#include <string.h>
#include "libtransport/http/v1/http_parser.h"

namespace libtransport {
namespace http {

static void stripTrailingBytes(std::string* buf, unsigned char byte) {
  auto begin = (const unsigned char*) buf->data();
  auto cur = begin + buf->size();

  while (cur > begin && *(cur - 1) == byte) {
    cur--;
  }

  buf->resize(cur - begin);
}

const char HTTPParser::kContentLengthHeader[] = "Content-Length";
const char HTTPParser::kConnectionHeader[] = "Connection";

HTTPParser::HTTPParser(
    kParserMode mode,
    size_t buffer_size /* = kDefaultBufferSize */) :
    on_method_cb_(nullptr),
    on_uri_cb_(nullptr),
    on_version_cb_(nullptr),
    on_status_code_cb_(nullptr),
    on_status_name_cb_(nullptr),
    on_header_cb_(nullptr),
    on_headers_complete_cb_(nullptr),
    on_body_chunk_cb_(nullptr),
    mode_(mode),
    body_bytes_read_(0),
    body_bytes_expected_(0),
    expect_body_(true) {
  switch (mode) {
    case PARSE_HTTP_REQUEST:
      state_ = S_REQ_METHOD;
      break;

    case PARSE_HTTP_RESPONSE:
      state_ = S_RES_VERSION;
      break;
  }

  buf_.reserve(buffer_size);
}

HTTPParser::kParserState HTTPParser::state() const {
  return state_;
}

void HTTPParser::onMethod(
    std::function<void(HTTPMessage::kHTTPMethod)> callback) {
  on_method_cb_ = callback;
}

void HTTPParser::onURI(
    std::function<void(const char* data, size_t size)> callback) {
  on_uri_cb_ = callback;
}

void HTTPParser::onVersion(
    std::function<void(const char* data, size_t size)> callback) {
  on_version_cb_ = callback;
}

void HTTPParser::onStatusCode(std::function<void(int)> callback) {
  on_status_code_cb_ = callback;
}

void HTTPParser::onStatusName(
    std::function<void(const char* data, size_t size)> callback) {
  on_status_name_cb_ = callback;
}

void HTTPParser::onHeader(std::function<void(
    const char* key,
    size_t key_len,
    const char* val,
    size_t val_len)> callback) {
  on_header_cb_ = callback;
}

void HTTPParser::onHeadersComplete(std::function<void()> callback) {
  on_headers_complete_cb_ = callback;
}

void HTTPParser::onBodyChunk(
    std::function<void(const char* data, size_t size)> callback) {
  on_body_chunk_cb_ = callback;
}

bool HTTPParser::parse(const char* data, size_t size) {
  const char* begin = data;
  const char* end = data + size;

  while (begin < end) {
    switch (state_) {
      case S_REQ_METHOD:
        if (!parseMethod(&begin, end)) {
          return false;
        }
        break;
      case S_REQ_URI:
        if (!parseURI(&begin, end)) {
          return false;
        }
        break;
      case S_REQ_VERSION:
        if (!parseRequestVersion(&begin, end)) {
          return false;
        }
        break;
      case S_RES_VERSION:
        if (!parseResponseVersion(&begin, end)) {
          return false;
        }
        break;
      case S_RES_STATUS_CODE:
        if (!parseResponseStatusCode(&begin, end)) {
          return false;
        }
        break;
      case S_RES_STATUS_NAME:
        if (!parseResponseStatusName(&begin, end)) {
          return false;
        }
        break;
      case S_HEADER:
        if (!parseHeader(&begin, end)) {
          return false;
        }
        break;
      case S_DONE:
        if (mode_ == PARSE_HTTP_REQUEST) {
          return false; // invalid trailing bytes
        }
        /* fallthrough */
      case S_BODY:
        if (!readBody(&begin, end)) {
          return false;
        }
        break;

    }
  }

  return true;
}

bool HTTPParser::eof() {
  switch (state_) {
    case S_REQ_METHOD:
    case S_REQ_URI:
    case S_REQ_VERSION:
    case S_RES_VERSION:
    case S_RES_STATUS_CODE:
    case S_RES_STATUS_NAME:
    case S_HEADER:
    case S_BODY:
      if (body_bytes_expected_ != -1 &&
          body_bytes_read_ < body_bytes_expected_) {
        return false; // unexpected end of file
      } else {
        state_ = S_DONE;
      }
    case S_DONE:
      return true;
  }

  return true;
}

bool HTTPParser::parseMethod(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    state_ = S_REQ_URI;
    (*begin)++;

    if (on_method_cb_) {
      if (buf_ == "CONNECT") {
        on_method_cb_(HTTPMessage::M_CONNECT);
        buf_.clear();
        return true;
      }

      if (buf_ == "DELETE") {
        on_method_cb_(HTTPMessage::M_DELETE);
        buf_.clear();
        return true;
      }

      if (buf_ == "GET") {
        on_method_cb_(HTTPMessage::M_GET);
        buf_.clear();
        return true;
      }

      if (buf_ == "HEAD") {
        on_method_cb_(HTTPMessage::M_HEAD);
        buf_.clear();
        return true;
      }

      if (buf_ == "OPTIONS") {
        on_method_cb_(HTTPMessage::M_OPTIONS);
        buf_.clear();
        return true;
      }

      if (buf_ == "POST") {
        on_method_cb_(HTTPMessage::M_POST);
        buf_.clear();
        return true;
      }

      if (buf_ == "PUT") {
        on_method_cb_(HTTPMessage::M_PUT);
        buf_.clear();
        return true;
      }

      if (buf_ == "TRACE"){
        on_method_cb_(HTTPMessage::M_TRACE);
        buf_.clear();
        return true;
      }

      return false; // invalid method
    }
  }

  if (buf_.size() > kMaxMethodSize) {
    return false; // method too large
  } else {
    return true;
  }
}

bool HTTPParser::parseURI(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    if (on_uri_cb_) {
      on_uri_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_REQ_VERSION;
    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxURISize) {
    return false; // uri too large
  } else {
    return true;
  }
}

bool HTTPParser::parseRequestVersion(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    stripTrailingBytes(&buf_, '\r');

    if (on_version_cb_) {
      on_version_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_HEADER;
    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxVersionSize) {
    return false; // version too large
  } else {
    return true;
  }
}

bool HTTPParser::parseResponseVersion(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    if (strncasecmp((char *) buf_.data(), "HTTP/1.0", buf_.size()) == 0) {
      body_bytes_expected_ = -1;
    }

    if (on_version_cb_) {
      on_version_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_RES_STATUS_CODE;
    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxVersionSize) {
    return false; // version too large
  } else {
    return true;
  }
}

bool HTTPParser::parseResponseStatusCode(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    std::string status_code_str((char *) buf_.data(), buf_.size());
    int status_code = 1;

    try {
      status_code = std::stoi(status_code_str);
    } catch (const std::exception& e) {
      return false; // invalid code
    }

    if (on_status_code_cb_) {
      on_status_code_cb_(status_code);
    }

    buf_.clear();
    state_ = S_RES_STATUS_NAME;
    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxURISize) {
    return false; // uri too large
  } else {
    return true;
  }
}

bool HTTPParser::parseResponseStatusName(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    stripTrailingBytes(&buf_, '\r');

    if (on_status_name_cb_) {
      on_status_name_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_HEADER;
    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxVersionSize) {
    return false; // too large
  } else {
    return true;
  }
}

bool HTTPParser::parseHeader(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    stripTrailingBytes(&buf_, '\r');

    if (buf_.size() > 0) {
      auto hkey = (char*) buf_.data();
      auto hkey_len = buf_.find(':');
      if (hkey_len == std::string::npos) {
        return false; // invalid header line
      }

      auto hval = hkey + hkey_len + 1;
      auto hval_len = buf_.size() - hkey_len - 1;
      while (hval_len > 0 && *hval == ' ') {
        hval++;
        hval_len--;
      }

      if (!processHeader(hkey, hkey_len, hval, hval_len)) {
        return false;
      }

      buf_.clear();
      state_ = S_HEADER;
    } else {
      if (body_bytes_expected_ == 0) {
        state_ = S_DONE;
      } else {
        state_ = S_BODY;
      }

      if (on_headers_complete_cb_) {
        on_headers_complete_cb_();
      }
    }

    (*begin)++;
    return true;
  }

  if (buf_.size() > kMaxHeaderSize) {
    return false; // header too large
  } else {
    return true;
  }
}

bool HTTPParser::processHeader(
    const char* key,
    size_t key_len,
    const char* val,
    size_t val_len) {
  if (expect_body_ &&
      key_len == strlen(kContentLengthHeader) &&
      strncasecmp(key, kContentLengthHeader, key_len) == 0) {
    std::string content_length_str(val, val_len);
    try {
      body_bytes_expected_ = std::stoul(content_length_str);
    } catch (const std::exception& e) {
      return false; // invalid content length
    }
  }

  if (mode_ == PARSE_HTTP_RESPONSE &&
      expect_body_ &&
      key_len == strlen(kConnectionHeader) &&
      strncasecmp(key, kConnectionHeader, key_len) == 0 &&
      val_len == strlen("Close") &&
      strncasecmp(val, "Close", val_len) == 0) {
    body_bytes_expected_ = -1;
  }

  if (on_header_cb_) {
    on_header_cb_(key, key_len, val, val_len);
  }

  return true;
}

bool HTTPParser::readBody(const char** begin, const char* end) {
  body_bytes_read_ += end - *begin;

  if (body_bytes_read_ == body_bytes_expected_) {
    state_ = HTTPParser::S_DONE;
  }

  //if ((body_bytes_read_ > body_bytes_expected_) &&
  //    !(body_bytes_expected_ == 0 && mode_ == PARSE_HTTP_RESPONSE)) {
  //  RAISE(kParseError, "invalid trailing body bytes");
  //}

  if (on_body_chunk_cb_) {
    on_body_chunk_cb_(*begin, end - *begin);
  }

  *begin = end;
  return true;
}

bool HTTPParser::readUntil(const char** begin, const char* end, char search) {
  auto cur = *begin;
  for (; cur < end && *cur != search; ++cur);
  buf_.append(*begin, (cur - *begin));
  *begin = cur;
  return cur != end && *cur == search;
}

void HTTPParser::reset() {
  switch (mode_) {
    case PARSE_HTTP_REQUEST:
      state_ = S_REQ_METHOD;
      break;

    case PARSE_HTTP_RESPONSE:
      state_ = S_RES_VERSION;
      break;
  }

  buf_.clear();
  body_bytes_read_ = 0;
  body_bytes_expected_ = 0;
  expect_body_ = true;
}

void HTTPParser::ignoreBody() {
  expect_body_ = false;
}

} // namespace http
} // namespace libtransport

