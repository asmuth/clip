/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _libstx_HTTP_PARSER_H
#define _libstx_HTTP_PARSER_H
#include <memory>
#include <vector>

#include "fnordmetric/util/buffer.h"
#include "fnordmetric/transport/http/httpmessage.h"

namespace fnordmetric {
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
  void parse(const char* data, size_t size);
  void eof();
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
  void parseMethod(const char** begin, const char* end);
  void parseURI(const char** begin, const char* end);
  void parseRequestVersion(const char** begin, const char* end);
  void parseResponseVersion(const char** begin, const char* end);
  void parseResponseStatusCode(const char** begin, const char* end);
  void parseResponseStatusName(const char** begin, const char* end);
  void parseHeader(const char** begin, const char* end);
  void readBody(const char** begin, const char* end);
  bool readUntil(const char** begin, const char* end, char search);
  void processHeader(
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
  Buffer buf_;
  size_t body_bytes_read_;
  size_t body_bytes_expected_;
  bool expect_body_;
};

}
}
#endif
