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
#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "libtransport/http/http_message.h"

namespace libtransport {
namespace http {

class HTTPParser {
public:
  static const size_t kDefaultBufferSize = 4096;
  static const size_t kMaxMethodSize = 16;
  static const size_t kMaxURISize = 8192;
  static const size_t kMaxVersionSize = 16;
  static const size_t kMaxHeaderSize = 65535;
  static const char kContentLengthHeader[];
  static const char kConnectionHeader[];

  enum kParserMode {
    PARSE_HTTP_REQUEST,
    PARSE_HTTP_RESPONSE
  };

  enum kParserState {
    S_REQ_METHOD = 1,
    S_REQ_URI = 2,
    S_REQ_VERSION = 3,
    S_RES_VERSION = 4,
    S_RES_STATUS_CODE = 5,
    S_RES_STATUS_NAME = 6,
    S_HEADER = 7,
    S_BODY = 8,
    S_DONE = 9
  };

  HTTPParser(kParserMode mode, size_t buffer_size = kDefaultBufferSize);

  kParserState state() const;
  bool parse(const char* data, size_t size);
  bool eof();
  void reset();
  void ignoreBody();

  void onMethod(std::function<void(HTTPMessage::kHTTPMethod)> callback);
  void onURI(std::function<void(const char* data, size_t size)> callback);
  void onVersion(std::function<void(const char* data, size_t size)> callback);
  void onStatusCode(std::function<void(int code)> callback);
  void onStatusName(
      std::function<void(const char* data, size_t size)> callback);
  void onHeader(std::function<void(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len)> callback);
  void onHeadersComplete(std::function<void()> callback);
  void onBodyChunk(std::function<void(const char* data, size_t size)> callback);

protected:
  bool parseMethod(const char** begin, const char* end);
  bool parseURI(const char** begin, const char* end);
  bool parseRequestVersion(const char** begin, const char* end);
  bool parseResponseVersion(const char** begin, const char* end);
  bool parseResponseStatusCode(const char** begin, const char* end);
  bool parseResponseStatusName(const char** begin, const char* end);
  bool parseHeader(const char** begin, const char* end);
  bool readBody(const char** begin, const char* end);
  bool readUntil(const char** begin, const char* end, char search);
  bool processHeader(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len);

  std::function<void(HTTPMessage::kHTTPMethod)> on_method_cb_;
  std::function<void(const char* data, size_t size)> on_uri_cb_;
  std::function<void(const char* data, size_t size)> on_version_cb_;
  std::function<void(int)> on_status_code_cb_;
  std::function<void(const char* data, size_t size)> on_status_name_cb_;
  std::function<void(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len)> on_header_cb_;
  std::function<void()> on_headers_complete_cb_;
  std::function<void(const char* data, size_t size)> on_body_chunk_cb_;

  kParserMode mode_;
  kParserState state_;
  std::string buf_;
  size_t body_bytes_read_;
  size_t body_bytes_expected_;
  bool expect_body_;
};

} // namespace http
} // namespace libtransport

