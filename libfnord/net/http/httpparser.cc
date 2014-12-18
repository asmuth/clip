/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/bufferutil.h"
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/net/http/httpparser.h"

namespace fnord {
namespace http {

HTTPParser::HTTPParser(
    size_t buffer_size /* = kDefaultBufferSize */) :
    state_(S_METHOD),
    on_method_cb_(nullptr),
    on_uri_cb_(nullptr),
    on_version_cb_(nullptr),
    on_header_cb_(nullptr),
    on_headers_complete_cb_(nullptr),
    body_bytes_read_(0),
    body_bytes_expected_(0) {
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

void HTTPParser::parse(const char* data, size_t size) {
  const char* begin = data;
  const char* end = data + size;

  while (begin < end) {
    switch (state_) {
      case S_METHOD:
        parseMethod(&begin, end);
        break;
      case S_URI:
        parseURI(&begin, end);
        break;
      case S_VERSION:
        parseVersion(&begin, end);
        break;
      case S_HEADER:
        parseHeader(&begin, end);
        break;
      case S_DONE:
        iputs("invalid trailng bytes: $0", std::string(begin, end - begin));
        RAISE(kParseError, "invalid trailing bytes");
    }
  }
}

void HTTPParser::eof() {
  switch (state_) {
    case S_METHOD:
    case S_URI:
    case S_VERSION:
    case S_HEADER:
    case S_BODY:
      if (body_bytes_read_ < body_bytes_expected_) {
        RAISE(kParseError, "unexpected end of file");
      }
      return;
    case S_DONE:
      return;
  }
}

void HTTPParser::parseMethod(const char** begin, const char* end) {
  if (readUntil(begin, end, ' ')) {
    state_ = S_URI;
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
    state_ = S_VERSION;
    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxURISize) {
    RAISEF(kParseError, "HTTP URI too large, max is $0", kMaxURISize);
  }
}

void HTTPParser::parseVersion(const char** begin, const char* end) {
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

      if (on_header_cb_) {
        on_header_cb_(hkey, hkey_len, hval, hval_len);
      }

      buf_.clear();
      state_ = S_HEADER;
    } else {
      if (on_headers_complete_cb_) {
        on_headers_complete_cb_();
      }

      if (body_bytes_expected_ == 0) {
        state_ = S_DONE;
      } else {
        state_ = S_BODY;
      }
    }

    (*begin)++;
    return;
  }

  if (buf_.size() > kMaxHeaderSize) {
    RAISEF(kParseError, "HTTP header too large, max is $0", kMaxHeaderSize);
  }
}

bool HTTPParser::readUntil(const char** begin, const char* end, char search) {
  auto cur = *begin;
  for (; cur < end && *cur != search; ++cur);
  buf_.append(*begin, (cur - *begin));
  *begin = cur;
  return *cur == search;
}

void HTTPParser::reset() {
  state_ = S_METHOD;
  buf_.clear();
  body_bytes_read_ = 0;
  body_bytes_expected_ = 0;
}

}
}
