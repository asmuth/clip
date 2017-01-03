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
#include "metricd/util/bufferutil.h"
#include "metricd/util/exception.h"
#include "metricd/util/inspect.h"
#include "metricd/transport/http/httpparser.h"

namespace fnordmetric {
namespace http {

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

void HTTPParser::parse(const char* data, size_t size) {
  const char* begin = data;
  const char* end = data + size;

  while (begin < end) {
    switch (state_) {
      case S_REQ_METHOD:
        parseMethod(&begin, end);
        break;
      case S_REQ_URI:
        parseURI(&begin, end);
        break;
      case S_REQ_VERSION:
        parseRequestVersion(&begin, end);
        break;
      case S_RES_VERSION:
        parseResponseVersion(&begin, end);
        break;
      case S_RES_STATUS_CODE:
        parseResponseStatusCode(&begin, end);
        break;
      case S_RES_STATUS_NAME:
        parseResponseStatusName(&begin, end);
        break;
      case S_HEADER:
        parseHeader(&begin, end);
        break;
      case S_DONE:
        if (mode_ == PARSE_HTTP_REQUEST) {
          RAISE(kParseError, "invalid trailing bytes");
        }
        /* fallthrough */
      case S_BODY:
        readBody(&begin, end);
        break;

    }
  }
}

void HTTPParser::eof() {
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
        RAISE(kParseError, "unexpected end of file");
      } else {
        state_ = S_DONE;
      }
      return;
    case S_DONE:
      return;
  }
}

void HTTPParser::parseMethod(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    state_ = S_REQ_URI;
    (*begin)++;

    if (on_method_cb_) {
      if (buf_ == "CONNECT") {
        on_method_cb_(HTTPMessage::M_CONNECT);
        buf_.clear();
        return;
      }

      if (buf_ == "DELETE") {
        on_method_cb_(HTTPMessage::M_DELETE);
        buf_.clear();
        return;
      }

      if (buf_ == "GET") {
        on_method_cb_(HTTPMessage::M_GET);
        buf_.clear();
        return;
      }

      if (buf_ == "HEAD") {
        on_method_cb_(HTTPMessage::M_HEAD);
        buf_.clear();
        return;
      }

      if (buf_ == "OPTIONS") {
        on_method_cb_(HTTPMessage::M_OPTIONS);
        buf_.clear();
        return;
      }

      if (buf_ == "POST") {
        on_method_cb_(HTTPMessage::M_POST);
        buf_.clear();
        return;
      }

      if (buf_ == "PUT") {
        on_method_cb_(HTTPMessage::M_PUT);
        buf_.clear();
        return;
      }

      if (buf_ == "TRACE"){
        on_method_cb_(HTTPMessage::M_TRACE);
        buf_.clear();
        return;
      }

      RAISEF(kParseError, "Invalid HTTP method: $0", buf_.toString());
    }
  }

  if (buf_.size() > kMaxMethodSize) {
    RAISEF(kParseError, "HTTP method too large, max is $0", kMaxMethodSize);
  }
}

void HTTPParser::parseURI(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    if (on_uri_cb_) {
      on_uri_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_REQ_VERSION;
    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxURISize) {
    RAISEF(kParseError, "HTTP URI too large, max is $0", kMaxURISize);
  }
}

void HTTPParser::parseRequestVersion(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    BufferUtil::stripTrailingBytes(&buf_, '\r');

    if (on_version_cb_) {
      on_version_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_HEADER;
    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxVersionSize) {
    RAISEF(kParseError, "HTTP version too large, max is $0", kMaxVersionSize);
  }
}

void HTTPParser::parseResponseVersion(const char** begin, const char* end) {
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
    return;
  }

  if (buf_.size() > kMaxVersionSize) {
    RAISEF(kParseError, "HTTP version too large, max is $0", kMaxVersionSize);
  }
}

void HTTPParser::parseResponseStatusCode(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    std::string status_code_str((char *) buf_.data(), buf_.size());
    int status_code = 1;

    try {
      status_code = std::stoi(status_code_str);
    } catch (const std::exception& e) {
      RAISEF(kParseError, "invalid http status code: $0", status_code_str);
    }

    if (on_status_code_cb_) {
      on_status_code_cb_(status_code);
    }

    buf_.clear();
    state_ = S_RES_STATUS_NAME;
    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxURISize) {
    RAISEF(kParseError, "HTTP URI too large, max is $0", kMaxURISize);
  }
}

void HTTPParser::parseResponseStatusName(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    BufferUtil::stripTrailingBytes(&buf_, '\r');

    if (on_status_name_cb_) {
      on_status_name_cb_((char *) buf_.data(), buf_.size());
    }

    buf_.clear();
    state_ = S_HEADER;
    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxVersionSize) {
    RAISEF(kParseError, "HTTP version too large, max is $0", kMaxVersionSize);
  }
}

void HTTPParser::parseHeader(const char** begin, const char* end) {
  if (readUntil(begin, end, '\n')) {
    BufferUtil::stripTrailingBytes(&buf_, '\r');

    if (buf_.size() > 0) {
      auto hkey = (char*) buf_.data();
      auto hkey_len = buf_.find(':');
      if (hkey_len == Buffer::npos) {
        RAISEF(kParseError, "invalid HTTP header line: $0", buf_.toString());
      }

      auto hval = hkey + hkey_len + 1;
      auto hval_len = buf_.size() - hkey_len - 1;
      while (hval_len > 0 && *hval == ' ') {
        hval++;
        hval_len--;
      }

      processHeader(hkey, hkey_len, hval, hval_len);
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
    return;
  }

  if (buf_.size() > kMaxHeaderSize) {
    RAISEF(kParseError, "HTTP header too large, max is $0", kMaxHeaderSize);
  }
}

void HTTPParser::processHeader(
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
      RAISEF(kParseError, "invalid content length: $0", content_length_str);
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
}

void HTTPParser::readBody(const char** begin, const char* end) {
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

}
}
