/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTP_PARSER_H
#define _FNORDMETRIC_HTTP_PARSER_H
#include <memory>
#include <vector>

#include "fnord/base/buffer.h"
#include "fnord/net/http/httpmessage.h"

namespace fnord {
namespace http {

class HTTPParser {
public:
  static const size_t kDefaultBufferSize = 4096;
  static const size_t kMaxMethodSize = 16;
  static const size_t kMaxURISize = 8192;
  static const size_t kMaxVersionSize = 16;
  static const size_t kMaxHeaderSize = 65535;
  static const char kContentLengthHeader[];

  HTTPParser(size_t buffer_size = kDefaultBufferSize);

  enum kParserState {
    S_METHOD = 1,
    S_URI = 2,
    S_VERSION = 3,
    S_HEADER = 4,
    S_BODY = 5,
    S_DONE = 6
  };

  kParserState state() const;
  void parse(const char* data, size_t size);
  void eof();
  void reset();

  void onMethod(std::function<void(HTTPMessage::kHTTPMethod)> callback);
  void onURI(std::function<void(const char* data, size_t size)> callback);
  void onVersion(std::function<void(const char* data, size_t size)> callback);
  void onHeader(std::function<void(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len)> callback);
  void onHeadersComplete(std::function<void()> callback);

protected:
  void parseMethod(const char** begin, const char* end);
  void parseURI(const char** begin, const char* end);
  void parseVersion(const char** begin, const char* end);
  void parseHeader(const char** begin, const char* end);
  bool readUntil(const char** begin, const char* end, char search);
  void processHeader(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len);

  std::function<void(HTTPMessage::kHTTPMethod)> on_method_cb_;
  std::function<void(const char* data, size_t size)> on_uri_cb_;
  std::function<void(const char* data, size_t size)> on_version_cb_;
  std::function<void(
      const char* key,
      size_t key_len,
      const char* val,
      size_t val_len)> on_header_cb_;
  std::function<void()> on_headers_complete_cb_;

  kParserState state_;
  Buffer buf_;
  size_t body_bytes_read_;
  size_t body_bytes_expected_;
};

}
}
#endif
